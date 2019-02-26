#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 65435

typedef struct frame
{
    int sq_no;
    int ack;
    char buffer[BUFFER_SIZE];
} Frame;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void tcp(char *filename)
{
    int sockfd, filefd, read_return;
    long nr_mesages = 0, nr_bytes = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];
    clock_t begin_time, end_time;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(HOST);
    if (server == NULL)
        error("ERROR, no such host");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    if ((filefd = open(filename, O_RDONLY)) < 0)
    {
        error("ERROR oppening file");
    }

    begin_time = clock();
    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        if ((read_return = read(filefd, buffer, BUFFER_SIZE)) < 0)
            error("ERROR readind from file");
        if (read_return == 0)
            break;
        if (write(sockfd, buffer, strlen(buffer)) < 0)
            error("ERROR writing to socket");

        nr_mesages++;
        nr_bytes += read_return;
    }
    end_time = clock();

    close(sockfd);
    close(filefd);

    printf("Transmission time: %f\n", ((double)(end_time - begin_time) / CLOCKS_PER_SEC));
    printf("Number of sent messages: %ld\n", nr_mesages);
    printf("Number of bytes sent: %ld\n", nr_bytes);
}

void udp(char *filename, int is_stream)
{
    int sockfd, filefd, read_return, len;
    long nr_mesages = 0, nr_bytes = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    clock_t begin_time, end_time;

    int frame_id = 0, ack_recv = 1;
    Frame frame_send, frame_recv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if ((filefd = open(filename, O_RDONLY)) < 0)
    {
        error("ERROR oppening file");
    }

    begin_time = clock();
    if (is_stream)
    {
        while (1)
        {
            bzero(buffer, BUFFER_SIZE);
            if ((read_return = read(filefd, buffer, BUFFER_SIZE)) < 0)
                error("ERROR readind from file");
            if (read_return == 0)
                break;
            if (sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR writing to socket");

            nr_mesages++;
            nr_bytes += read_return;
        }
        strcpy(buffer, "STOP");
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));
    }
    else
    {
        while (1)
        {
            if (ack_recv == 1)
            {
                frame_send.sq_no = frame_id;
                frame_send.ack = 0;

                if ((read_return = read(filefd, frame_send.buffer, BUFFER_SIZE)) < 0)
                    error("ERROR readind from file");
                if (read_return == 0)
                    break;

                sendto(sockfd, &frame_send, sizeof(Frame), MSG_CONFIRM, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            }

            len = sizeof(serv_addr);
            read_return = recvfrom(sockfd, &frame_recv, sizeof(Frame), MSG_WAITALL, (struct sockaddr *)&serv_addr, &len);

            if (read_return > 0 && frame_recv.sq_no == 0 && frame_recv.ack == frame_id + 1)
            {
                ack_recv = 1;
                printf("ACK received\n");
            }
            else
            {
                ack_recv = 0;
                printf("ACK not received\n");
            }

            nr_mesages++;
            nr_bytes += read_return;
        }
    }
    end_time = clock();

    close(sockfd);
    close(filefd);

    printf("Transmission time: %f\n", ((double)(end_time - begin_time) / CLOCKS_PER_SEC));
    printf("Number of sent messages: %ld\n", nr_mesages);
    printf("Number of bytes sent: %ld\n", nr_bytes);

    close(sockfd);
}

int main(int argc, char *argv[])
{
    int isTcp, isStream;

    if (argc < 3 || (strcasecmp(argv[1], "tcp") != 0 && strcasecmp(argv[1], "udp") != 0) || (strcasecmp(argv[2], "stream") != 0 && strcasecmp(argv[2], "saw") != 0))
    {
        fprintf(stderr, "usage: %s tcp/udp stream/saw filename\n", argv[0]);
        exit(0);
    }

    isTcp = strcasecmp(argv[1], "tcp") == 0;
    isStream = strcasecmp(argv[2], "stream") == 0;

    if (isTcp)
        tcp(argv[3]);
    else
    {
        udp(argv[3], isStream);
    }

    return 0;
}