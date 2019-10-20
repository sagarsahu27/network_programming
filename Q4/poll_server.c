#include "common.h"
#include <ctype.h>
#include <pthread.h>

#define MAX_SOCKET_CLIENT 10
SOCKET restrict_sockets[3];

/*
is_poll_available is a flag tell status of poll
0 -> No active poll
1 -> Recevied poll query from Host
2 -> Poll query sent to client and waiting for response
*/

int is_poll_available = 0;

struct arg
{
    fd_set* master;
    char* query;
    SOCKET* max_socket;
    size_t * query_len;
};


void* host_query_handler(void * argument) {

    struct arg* args = (struct arg* ) argument;
    while(1) {
        if (is_poll_available == 1) {
            for(int i = 1; i <= *(args->max_socket); ++i) {
                if (FD_ISSET(i, args->master)) {
                    if (i == restrict_sockets[0] || i == restrict_sockets[1] || i == restrict_sockets[2]) {
                        continue; // these are not clients sockets
                    }
                    // Send query to client
                    printf("Sending Query: %s\n", args->query);
                    send(i, args->query, *(args->query_len), 0);
                }
            }
            is_poll_available = 2;
            printf("Poll Started\n");
            int i = 60;
            while(i) {
                sleep(1);
                i -= 1;
                printf("\rTime remaining: %2d", i);
                fflush(stdout);
            } 
            is_poll_available = 0;
            printf("\nPoll ended\n");
            printf("Sending Poll response %s\n", args->query);
            for(int i = 1; i <= *(args->max_socket); ++i) {
                if (FD_ISSET(i, args->master)) {
                    if (i == restrict_sockets[0] || i == restrict_sockets[1]) {
                        continue; // these are not clients/host sockets
                    }
                    send(i, args->query, *(args->query_len), 0);
                }
            }
        }
    }
}

int main() {

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // For host port

    // Port where server will listen
    char port_host[8] = "5050\0" ;
    struct addrinfo *bind_address;
    getaddrinfo(0, port_host, &hints, &bind_address);

    // Socket creation
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!IsValidSocket(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    // socket bind to port
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);

    // Start listening to a single host on the port 5050
    if (listen(socket_listen, 1) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    SOCKET socket_listen_host = socket_listen;

    printf("Listening host on port %s\n", port_host);

    // For Clients port

    // Port where server will listen
    char port_client[8] = "5150\0" ;
    getaddrinfo(0, port_client, &hints, &bind_address);

    // Socket creation
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!IsValidSocket(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    // socket bind to port
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(bind_address);

    // Start listening to clients on the port 5150
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    printf("Listening Clients on port %s\n", port_client);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    FD_SET(socket_listen_host, &master);
    SOCKET max_socket = socket_listen, socket_host;
    restrict_sockets[0] = socket_listen_host;
    restrict_sockets[1] = socket_listen;
    char read[8192];
    size_t bytes_received = 0;
    int offset = 0;
    struct arg args;
    args.master = &master;
    args.max_socket = &max_socket;
    args.query = read;
    args.query_len = &bytes_received;
    int num_client = 0;

    pthread_t host_thread;

    if(pthread_create(&host_thread, NULL, host_query_handler, (void *)&args)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+2, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {

                // New client request
                if (i == socket_listen) {

                    // Accept Client request
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!IsValidSocket(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                errno);
                        return 1;
                    }

                    // Map Client to socket list
                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;
                    num_client++;
                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    printf("Connected with client %s with socket %d\n", address_buffer, socket_client);

                } else  if (i == socket_listen_host) {   // New host request

                    // Accept Client request
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    socket_host = accept(socket_listen_host,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!IsValidSocket(socket_host)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                errno);
                        return 1;
                    }

                    // Map Client to socket list
                    FD_SET(socket_host, &master);
                    if (socket_host > max_socket)
                        max_socket = socket_host;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    restrict_sockets[2] = socket_host;
                    printf("Connected with host with socket %d clients #%d\n", socket_host, num_client);
                    char rsp[64] = "Number of connected client: ";
                    char nc[3];
                    snprintf(nc, 3, "%d", num_client);
                    printf("nc %s\n", nc);
                    strncpy(rsp + 28, nc, 3);
                    strncpy(rsp + 31, " \0", 2);
                    printf("%s\n", rsp);
                    send(socket_host, rsp, strlen(rsp), 0);

                } else if (i == socket_host) {

                    // Receive request from exisitng host
                    bytes_received = recv(i, read, 8192, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CloseSocket(i);
                        continue;
                    }

                    // To prevent running command other than client provided
                    // due to buffer overflow
                    read[bytes_received] = '\0';
                    printf("Query received from %d: %s", i, read);

                    is_poll_available = 1;
                } else {

                    // Receive request from exisitng client
                    char resp[2];
                    int bytes_rec = recv(i, resp, 2, 0);
                    if (bytes_rec < 1) {
                        FD_CLR(i, &master);
                        CloseSocket(i);
                        num_client--;
                        continue;
                    }

                    if (is_poll_available <= 1) {
                        continue;
                    }
                    // To prevent running command other than client provided
                    // due to buffer overflow
                    resp[bytes_rec] = '\0';
                    printf("\nResponse received from %d: %s\n", i,resp);
                    char client_id[3];
                    snprintf(client_id, 10, "%d", i);
                    int str_len = strlen(resp);
                    strncpy(read + bytes_received ,"\n Client ",9);
                    strncpy(read + bytes_received + 8, client_id ,2);
                    strncpy(read + bytes_received + 10," - ",3);
                    strncpy(read + bytes_received + 13,resp,1);
                    strncpy(read + bytes_received + 14,"\0",1);
                    bytes_received += 14;
                    
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)

    if(pthread_join(host_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    printf("Closing listening socket...\n");
    CloseSocket(socket_listen);

    return 0;
}