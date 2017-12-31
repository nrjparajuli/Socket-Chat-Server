/* NIRAJ PARAJULI
 * Socket Chat Server
   Internet domain TCP client. 
   Usage: ./client <hostname> <port> 
   Really only useful with server
*/

#include "c-s-socket.h"

void* protocol(void* sockedfd);

//Global Variables
int clients[10]; // Accepts at most 10 clients. Can be changed.
char userName[10][256]; // Stores userName for clients

int main(int argc, char* argv[]){
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int i; // Loop Variable

    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    // Default name for Clients
    for (i = 0; i < 10; i++)
        sprintf(userName[i], "Unknown User %d", i);

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen)) {
        pthread_t thread_x;
        pthread_create(&thread_x, NULL, protocol, &newsockfd);
    }
    close(newsockfd);
    close(sockfd);

    return (0);
}

void* protocol(void* newsocketfd){

    int connected = 1;
    char message[256], username[256], buff[256];
    char quit[256], post[256], who[256], help[256];
    int n, i;
    int sockfd = *(int*)newsocketfd;
    clients[sockfd] = 1;

    strcpy(quit, "QUIT");
    strcpy(username, "USER");
    strcpy(post, "POST");
    strcpy(who, "WHO");
    strcpy(help, "HELP");

    for (i = 0; i < 10; i++) {
        if (clients[i]) {
            strcpy(message, "New user has connected");
            n = write(i, message, strlen(message));
            strcpy(message, "");
        }
    }

    while (connected) {
        bzero(buff, 256);
        n = read(sockfd, buff, 255);
        if (n < 0)
            error("ERROR reading from socket");

        if (strncmp(username, buff, strlen(username)) == 0) { //USER command
            memset(userName[sockfd], 0, strlen(userName[sockfd]));
            strncpy(userName[sockfd], buff + 5, strlen(buff) - 5);
        } else if (strncmp(post, buff, strlen(username)) == 0) { //POST command
            for (i = 0; i < 10; i++) {
                if (clients[i] == 1) {
                    strcpy(message, "");
                    strcat(message, userName[sockfd]);
                    strcat(message, ": ");
                    strcat(message, buff + 5);
                    n = write(i, message, strlen(message));
                }
            }
        } else if (strncmp(who, buff, strlen(username)) == 0) { // WHO command
            strcpy(message, "");
            strcat(message, "Logged in Users are:\n");
            for (i = 0; i < 10; i++) {
                if (clients[i] == 1) {
                    strcat(message, userName[i]);
		    strcat(message, "\n");
		}
	    }
            n = write(sockfd, message, strlen(message));
        } else if (strncmp(help, buff, strlen(username)) == 0) { //HELP command
            strcpy(message, "");
            strcat(message, "Some commands that you can use are:\n");
            strcat(message, "'USER SP Name CRLF'; login with the given name\n");
            strcat(message, "'POST SP Message CRLF'; post message to all users\n");
            strcat(message, "'WHO CRLF'; get list of logged in users\n");
            strcat(message, "'HELP CRLF'; show list of operations\n");
            strcat(message, "'QUIT CRLF'; disconnect from the server");
            n = write(sockfd, message, strlen(message));
        } else if (strncmp(quit, buff, strlen(quit)) == 0) { //QUIT command
	    for (i = 0; i < 10; i++) {
	        if (clients[i] && i != sockfd) {
		    sprintf(message, "%s disconnected", userName[sockfd]);
		    n = write(i, message, strlen(message));
		}
	    }
	    sprintf(userName[sockfd], "Unknown user %d", sockfd);
	    printf("Closing connection over sockfd: %d \n", sockfd);
	    clients[sockfd] = 0;
            connected = 0;
            strcpy(message, "/QUIT");
            n = write(sockfd, message, strlen(message));
        }
    }
    close(sockfd);
}
