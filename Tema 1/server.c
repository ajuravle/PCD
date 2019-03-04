#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BACKLOG 5
#define BUFFER_SIZE 1024

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void tcp(int sockfd, int isSaw)
{
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    int newsockfd, return_value;
    long nr_mesages = 0, nr_bytes = 0;
    char buffer[BUFFER_SIZE];
    char* ack = "ack";

    if (listen(sockfd, BACKLOG) < 0)
        error("ERROR on listening");

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        if ((return_value = read(newsockfd, buffer, BUFFER_SIZE)) < 0)
            error("ERROR reading from socket");

        if (strcmp(buffer, "STOP") == 0)
            break;

        if (isSaw)
        {
            if (write(newsockfd, ack, strlen(ack)) < 0)
                error("ERROR writting to socket");
        }

        nr_mesages++;
        nr_bytes += return_value;
    }

    close(newsockfd);

    printf("Used protocol: TCP\n");
    printf("Number of messages read: %ld\n", nr_mesages);
    printf("Number of bytes read: %ld\n", nr_bytes);
}

void udp(int sockfd, int isSaw)
{
    struct sockaddr_in cli_addr;
    int return_value, cli_addr_len;
    long nr_mesages = 0, nr_bytes = 0;
    char buffer[BUFFER_SIZE];
    char* ack = "ack";

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        if ((return_value = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0)
            error("ERROR reading from socket");

        if (strcmp(buffer, "STOP") == 0)
            break;

        if (isSaw)
        {
            if ((return_value = sendto(sockfd, ack, strlen(ack), MSG_CONFIRM, (const struct sockaddr *)&cli_addr, cli_addr_len)) < 0)
                error("ERROR writing to socket");
        }

        nr_mesages++;
        nr_bytes += return_value;
    }
    recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cli_addr, &cli_addr_len);

    printf("Used protocol: UDP\n");
    printf("Number of messages read: %ld\n", nr_mesages);
    printf("Number of bytes read: %ld\n", nr_bytes);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int sockfd, isTcp, isSaw;

    if (argc < 3 || (strcasecmp(argv[1], "tcp") != 0 && strcasecmp(argv[1], "udp") != 0) || (strcasecmp(argv[2], "stream") != 0 && strcasecmp(argv[2], "saw") != 0))
    {
        fprintf(stderr, "usage: %s tcp/udp stream/saw\n", argv[0]);
        exit(0);
    }

    isTcp = strcasecmp(argv[1], "tcp") == 0;
    isSaw = strcasecmp(argv[2], "saw") == 0;

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
        tcp(sockfd, isSaw);
    else
        udp(sockfd, isSaw);

    close(sockfd);

    return 0;
}