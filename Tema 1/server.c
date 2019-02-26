#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BACKLOG 5
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
    exit(1);
}

void tcp(int sockfd)
{
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    int newsockfd, read_return;
    long nr_mesages = 0, nr_bytes = 0;
    char buffer[BUFFER_SIZE];

    if (listen(sockfd, BACKLOG) < 0)
        error("ERROR on listening");

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        if ((read_return = read(newsockfd, buffer, BUFFER_SIZE)) < 0)
            error("ERROR reading from socket");
        if (read_return == 0)
            break;

        nr_mesages++;
        nr_bytes += read_return;
    }

    close(newsockfd);

    printf("Used protocol: TCP\n");
    printf("Number of messages read: %ld\n", nr_mesages);
    printf("Number of bytes read: %ld\n", nr_bytes);
}

void udp(int sockfd, int is_stream)
{
    struct sockaddr_in cli_addr;
    int read_return, cli_addr_len;
    long nr_mesages = 0, nr_bytes = 0;
    char buffer[BUFFER_SIZE];

    int frame_id = 0;
    Frame frame_send, frame_recv;

    if (is_stream)
    {
        while (1)
        {
            bzero(buffer, BUFFER_SIZE);
            if ((read_return = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0)
                error("ERROR reading from socket");
            
            if(strcmp(buffer, "STOP") == 0)
                break;

            nr_mesages++;
            nr_bytes += read_return;
        }
    }
    else
    {
        while (1)
        {
            read_return = recvfrom(sockfd, &frame_recv, sizeof(Frame), MSG_WAITALL, (struct sockaddr *)&cli_addr, &cli_addr_len);
            if (read_return > 0 && frame_recv.sq_no == frame_id)
            {
                frame_send.sq_no = 0;
                frame_send.ack = frame_recv.sq_no + 1;
                bzero(frame_send.buffer, BUFFER_SIZE);

                if (sendto(sockfd, (const char *)&frame_send, sizeof(Frame), MSG_CONFIRM, (struct sockaddr *)&cli_addr, cli_addr_len) < 0)
                    error("ERROR sending to socket");

                printf("ACK Sent\n");
            }
            else
            {
                printf("PCK not received \n");
            }
            frame_id++;

            nr_mesages++;
            nr_bytes += read_return;
        }
    }

    printf("Used protocol: UDP\n");
    printf("Number of messages read: %ld\n", nr_mesages);
    printf("Number of bytes read: %ld\n", nr_bytes);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int sockfd, isTcp, isStream;

    if (argc < 3 || (strcasecmp(argv[1], "tcp") != 0 && strcasecmp(argv[1], "udp") != 0) || (strcasecmp(argv[2], "stream") != 0 && strcasecmp(argv[2], "saw") != 0))
    {
        fprintf(stderr, "usage: %s tcp/udp stream/saw\n", argv[0]);
        exit(0);
    }

    isTcp = strcasecmp(argv[1], "tcp") == 0;
    isStream = strcasecmp(argv[2], "stream") == 0;

    sockfd = socket(AF_INET, isTcp ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    if (isTcp)
        tcp(sockfd);
    else
        udp(sockfd, isStream);

    close(sockfd);

    return 0;
}