#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    int sock;
    char username[32];
} client_t;

int client_sockets[MAX_CLIENTS] = {0};
pthread_mutex_t client_sockets_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *client_struct) {
    client_t *client_info = (client_t *)client_struct;
    int read_size;
    char client_message[1024], broadcast_message[1056];

    // Receive username
    if((read_size = recv(client_info->sock, client_info->username, 32, 0)) > 0) {
        client_info->username[read_size] = '\0'; // Null-terminate the username
        printf("%s joined the chat\n", client_info->username);
    }

    // Receive messages from client
    while((read_size = recv(client_info->sock, client_message, 1024, 0)) > 0) {
        client_message[read_size] = '\0';
        pthread_mutex_lock(&client_sockets_mutex);
        sprintf(broadcast_message, "%s: %s", client_info->username, client_message);
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(client_sockets[i] != 0 && client_sockets[i] != client_info->sock) {
                send(client_sockets[i], broadcast_message, strlen(broadcast_message), 0);
            }
        }
        pthread_mutex_unlock(&client_sockets_mutex);
    }

    if(read_size == 0) {
        printf("%s disconnected\n", client_info->username);
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }

    // Remove the socket from the list
    pthread_mutex_lock(&client_sockets_mutex);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(client_sockets[i] == client_info->sock) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&client_sockets_mutex);

    free(client_struct);
    return 0;
}

int main() {
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
    listen(socket_desc, MAX_CLIENTS);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");

        client_t *new_client = malloc(sizeof(client_t));
        new_client->sock = new_socket;

        pthread_mutex_lock(&client_sockets_mutex);
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&client_sockets_mutex);

        if(pthread_create(&thread_id, NULL, client_handler, (void*) new_client) < 0) {
            perror("could not create thread");
            return 1;
        }
    }

    if(new_socket < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}
