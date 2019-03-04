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
#define BUFFER_SIZE 1024

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void tcp(char *filename, int isSaw)
{
    int sockfd, filefd, return_value;
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
        if ((return_value = read(filefd, buffer, BUFFER_SIZE)) < 0)
            error("ERROR readind from file");
        if (return_value == 0)
            break;
        if ((return_value = write(sockfd, buffer, strlen(buffer))) < 0)
            error("ERROR writing to socket");

        if (isSaw)
        {
            bzero(buffer, BUFFER_SIZE);
            if (read(sockfd, buffer, BUFFER_SIZE) < 0)
                error("ERROR reading from socket");
        }

        nr_mesages++;
        nr_bytes += return_value;
    }
    strcpy(buffer, "STOP");
    write(sockfd, buffer, strlen(buffer));

    end_time = clock();

    close(sockfd);
    close(filefd);

    printf("Transmission time: %f\n", ((double)(end_time - begin_time) / CLOCKS_PER_SEC));
    printf("Number of sent messages: %ld\n", nr_mesages);
    printf("Number of bytes sent: %ld\n", nr_bytes);
}

void udp(char *filename, int isSaw)
{
    int sockfd, filefd, return_value, len;
    long nr_mesages = 0, nr_bytes = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    clock_t begin_time, end_time;

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

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        if ((return_value = read(filefd, buffer, BUFFER_SIZE)) < 0)
            error("ERROR readind from file");
        if (return_value == 0)
            break;
        if ((return_value = sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
            error("ERROR writing to socket");

        if (isSaw)
        {
            bzero(buffer, BUFFER_SIZE);
            if ((recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&serv_addr, &len)) < 0)
                error("ERROR reading from socket");
        }

        nr_mesages++;
        nr_bytes += return_value;
    }
    strcpy(buffer, "STOP");
    sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

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
    int isTcp, isSaw;

    if (argc < 3 || (strcasecmp(argv[1], "tcp") != 0 && strcasecmp(argv[1], "udp") != 0) || (strcasecmp(argv[2], "stream") != 0 && strcasecmp(argv[2], "saw") != 0))
    {
        fprintf(stderr, "usage: %s tcp/udp stream/saw filename\n", argv[0]);
        exit(0);
    }

    isTcp = strcasecmp(argv[1], "tcp") == 0;
    isSaw = strcasecmp(argv[2], "saw") == 0;

    if (isTcp)
        tcp(argv[3], isSaw);
    else
    {
        udp(argv[3], isSaw);
    }

    return 0;
}