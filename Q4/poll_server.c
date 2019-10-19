#include "common.h"
#include <ctype.h>

#define MAX_SOCKET_CLIENT 10

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
                    SOCKET socket_host = accept(socket_listen_host,
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
                    printf("Connected with host with socket %d\n", socket_host);

                } else if (i == socket_host) {

                    // Receive request from exisitng host
                    char read[8192];
                    int bytes_received = recv(i, read, 8192, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CloseSocket(i);
                        continue;
                    }

                    FILE *cmd_pipe;
                    int bytes_read;
                    long unsigned int nbytes = 8192;
                    char cmd_output[nbytes];

                    // To prevent running command other than client provided
                    // due to buffer overflow
                    read[bytes_received] = '\0';
                    printf("Command received from %d: %s", i, read);

                    // Open cmd pipes to pass client command 
                    cmd_pipe = popen(read, "r");

                    // Check that pipes are non-null 
                    if (!cmd_pipe) {
                        fprintf (stderr, "pipes failed.\n");
                        return 1;
                    }

                    // Read output from cmd_pipe 
                    char tmp_str[256];
                    int offset =0;
                    memset(tmp_str,0,256);
                    memset(cmd_output,0,8192);
                    while(fgets(tmp_str, 256, cmd_pipe) != NULL) {
                        int str_len = strlen(tmp_str);
                        strncpy(cmd_output + offset,tmp_str,str_len);
                        offset += str_len;
                    }

                    // Close cmd_pipe, checking for errors 
                    if (pclose(cmd_pipe) != 0) {
                        fprintf (stdout, "Could not run %i, or other error.\n", errno);
                    }

                    // Send reply to client
                    printf("Sending Reply: %s\n", cmd_output);
                    send(i, cmd_output, nbytes, 0);

                } else {

                    // Receive request from exisitng client
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CloseSocket(i);
                        continue;
                    }

                    FILE *cmd_pipe;
                    int bytes_read;
                    long unsigned int nbytes = 8192;
                    char cmd_output[nbytes];

                    // To prevent running command other than client provided
                    // due to buffer overflow
                    read[bytes_received] = '\0';
                    printf("Command received from %d: %s", i, read);

                    // Open cmd pipes to pass client command 
                    cmd_pipe = popen(read, "r");

                    // Check that pipes are non-null 
                    if (!cmd_pipe) {
                        fprintf (stderr, "pipes failed.\n");
                        return 1;
                    }

                    // Read output from cmd_pipe 
                    char tmp_str[256];
                    int offset =0;
                    memset(tmp_str,0,256);
                    memset(cmd_output,0,8192);
                    while(fgets(tmp_str, 256, cmd_pipe) != NULL) {
                        int str_len = strlen(tmp_str);
                        strncpy(cmd_output + offset,tmp_str,str_len);
                        offset += str_len;
                    }

                    // Close cmd_pipe, checking for errors 
                    if (pclose(cmd_pipe) != 0) {
                        fprintf (stdout, "Could not run %i, or other error.\n", errno);
                    }

                    // Send reply to client
                    printf("Sending Reply: %s\n", cmd_output);
                    send(i, cmd_output, nbytes, 0);
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)

    printf("Closing listening socket...\n");
    CloseSocket(socket_listen);

    return 0;
}