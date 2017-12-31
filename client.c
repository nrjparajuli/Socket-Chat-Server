/* NIRAJ PARAJULI
 * Socket Chat Client
   Internet domain TCP client. 
   Usage: ./client <hostname> <port> 
   Really only useful with server
*/

#include "c-s-socket.h"

void* wait(void* newsockfd);

int main(int argc, char* argv[]){
    int sockfd, portno, n, counter = 0;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int connected = 1;
    char buffer[256], buff[256];
    char quit[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL)
        error("ERROR no such host");

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    puts("Connection established");
    pthread_t thread_x;
    pthread_create(&thread_x, NULL, wait, &sockfd);

    strcpy(quit, "/QUIT");
    while (connected == 1) {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        if (strcmp(buffer, quit) == 0) {
            connected = 0;
        }
        else 
            printf("%s\n", buffer);
    }
    puts("Disconnecting socket");
    close(sockfd);
    return (0);
}

void* wait(void* newsockfd){
    int connected = 1;
    char buffer[256], buff[256];
    int n;
    int sockfd = *(int*)newsockfd;
    int i;

    while (connected == 1) {
        bzero(buffer, 256);
        fgets(buff, 255, stdin);
        strncpy(buffer, buff, strlen(buff) - 1);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
    }
}
