#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *sendMessage(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char message[1024];

    while(1) {
        fgets(message, 1024, stdin);
        if(send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 0;
        }
    }
}

void *receiveMessage(void *socket_desc) {
    int sock = *(int*)socket_desc, read_size;
    char message[1024];

    while(1) {
        if((read_size = recv(sock, message, 1024, 0)) > 0) {
            message[read_size]             = '\0';
            printf("%s", message);
            fflush(stdout); // Ensure the message is displayed immediately
        }
    }
}

int main() {
    int sock;
    struct sockaddr_in server;
    pthread_t send_thread, receive_thread;
    char username[32];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("Could not create socket\n");
        return -1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to the server
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    printf("Connected to server\n");
    printf("Enter your username: ");
    fgets(username, 32, stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline character
    send(sock, username, strlen(username), 0); // Send username to server

    // Create thread for sending messages
    if(pthread_create(&send_thread, NULL, sendMessage, (void*)&sock) < 0) {
        perror("could not create thread for sending");
        return 1;
    }

    // Create thread for receiving messages
    if(pthread_create(&receive_thread, NULL, receiveMessage, (void*)&sock) < 0) {
        perror("could not create thread for receiving");
        return 1;
    }

    // Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(sock);
    return 0;
}

