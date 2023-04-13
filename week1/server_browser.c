#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    // Check enough arguments
    if (argc != 2)
    {
        printf("Missing arguments\n");
        exit(1);
    }

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1)
    {
        perror("Create socket failed: ");
        exit(1);
    }

    // Create struct sockaddr
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    // Bind socket to sockaddr
    if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Binding Failed\n");
        exit(1);
    }

    // Listen 
    if (listen(serverSocket, 5) == -1)
    {
        printf("Listening Failed\n");
        exit(1);
    }
    printf("Waiting for client connecting ...\n");

    // Accept connection
    struct sockaddr_in clientAddr;
    int clientAddrLength = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLength);
    if (clientSocket == -1)
    {
        printf("Accept connection failed\n");
        exit(1);
    }
    printf("Client has connected: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    
    // Receive Message From Browser
    char buff[512];
    char *message = NULL;
    int size;
    while (1)
    {
        int ret = recv(clientSocket, buff, sizeof(buff), 0);
        if (ret <= 0) break;

        message = realloc(message, size + ret + 1);
        memcpy(message + size, buff, ret);
        size += ret;

        if (strstr(buff, "\r\n\r\n") == 0) break;
    }
    printf("%s", message);
    free(message);

    // Send message to browser
    char *m = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello World</h1></body></html>";
    if (send(clientSocket, m, strlen(m), 0) == -1)
    {
        printf("Send Failed\n");
        exit(1);
    }

    // Close
    close(clientSocket);
    close(serverSocket);
    printf("Socket closed\n");
    return 0;
}