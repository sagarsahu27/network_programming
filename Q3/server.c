#include "common.h"
#include <ctype.h>
#include <stdlib.h>

void processSingleServer(SOCKET socket_listen, int max_socket_client) {
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;
    int num_socket_client = 0;
    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return ;
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
                        return ;
                    }

                    // Check if number of client exceeds Maximum client limit
                    if(num_socket_client == max_socket_client) {
                        printf("Maximum client capacity is reached !!!\n");
                        CloseSocket(socket_client);
                        continue;
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
                    num_socket_client++;
                    printf("Connected with client %s with socket %d\n", address_buffer, socket_client);

                } else {

                    // Receive request from exisitng client
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CloseSocket(i);
                        exit(0);
                    }

    char *sptr;
    char seps[] = " \t";
    sptr = strtok(read, seps);
    int num1, num2;
    char op, result[128];
    if (sptr != NULL ) {
        num1 = atoi(sptr);
        if(num1 == 0 && sptr[0]!='0') {
            strcpy(result, "first operand invalid");
            goto REPLY;        
        }
    } else {
        strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
        goto REPLY;
    }

    sptr = strtok(NULL, seps);
    if (sptr != NULL ) {
        op = *sptr;
    } else {
        strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
        goto REPLY;
    }

    sptr = strtok(NULL, seps);
    if (sptr != NULL ) {
        for(int i = 0; sptr[i] != '\0'; i++){
            if(sptr[i] == '\n') {
                sptr[i] = '\0';
            }
        }
        num2 = atoi(sptr);
        if(num2 == 0 && sptr[0]!='0') {
            strcpy(result, "second operand invalid");
            goto REPLY;        
        }
    } else {
        strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
        goto REPLY;
    }


    switch (op)
    {
    case '+':
        num1 += num2;
        snprintf(result, 10, "%d", num1);
        break;
    case '-':
        num1 -= num2;
        snprintf(result, 10, "%d", num1);
    break;
    case '*':
        num1 *= num2;
        snprintf(result, 10, "%d", num1);
    break;    
    case '/':
        num1 /= num2;
        snprintf(result, 10, "%d", num1);
    break;    
    default:
        strcpy(result, "Unsupported Operator");
        break;
    }

REPLY:
                    // Send reply to client
                    printf("Sending Reply: %s\n", result);
                    send(i, result, 128, 0);
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)

}

void processMultiServer(SOCKET socket_listen, int max_socket_client) {

    int num_socket_client = 0;

    while(1) {
        // Accept Client request
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        SOCKET socket_client = accept(socket_listen,
            (struct sockaddr*) &client_address, &client_len);
        if (!IsValidSocket(socket_client)) {
            fprintf(stderr, "accept() failed. (%d)\n", errno);
            return ;
        }

        char address_buffer[100];
        getnameinfo((struct sockaddr*)&client_address, client_len,
            address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
        num_socket_client++;
        printf("Connected with client %s with socket %d\n", address_buffer, socket_client);

        int pid = fork();

        if (pid == 0) { //child process
            CloseSocket(socket_listen);
            while(1) {
                // Receive request from exisitng client
                char read[1024];
                int bytes_received = recv(socket_client, read, 1024, 0);
                if (bytes_received < 1) {
                    CloseSocket(socket_client);
                    continue;
                }

                char *sptr;
                char seps[] = " \t";
                sptr = strtok(read, seps);
                int num1, num2;
                char op, result[128];
                if (sptr != NULL ) {
                    num1 = atoi(sptr);
                    if(num1 == 0 && sptr[0]!='0') {
                        strcpy(result, "first operand invalid");
                        goto REPLY;        
                    }
                } else {
                    strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
                    goto REPLY;
                }
            
                sptr = strtok(NULL, seps);
                if (sptr != NULL ) {
                    op = *sptr;
                } else {
                    strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
                    goto REPLY;
                }
            
                sptr = strtok(NULL, seps);
                if (sptr != NULL ) {
                    for(int k = 0; sptr[k] != '\0'; k++){
                        if(sptr[k] == '\n') {
                            sptr[k] = '\0';
                        }
                    }
                    num2 = atoi(sptr);
                    if(num2 == 0 && sptr[0]!='0') {
                        strcpy(result, "second operand invalid");
                        goto REPLY;        
                    }
                } else {
                    strcpy(result, "invalid expression\n Usage: Operand1 <SPACE> Operator(+,-,*,/) <SPACE> Operand2\n\n");
                    goto REPLY;
                }
            
            
                switch (op)
                {
                case '+':
                    num1 += num2;
                    snprintf(result, 10, "%d", num1);
                    break;
                case '-':
                    num1 -= num2;
                    snprintf(result, 10, "%d", num1);
                break;
                case '*':
                    num1 *= num2;
                    snprintf(result, 10, "%d", num1);
                break;    
                case '/':
                    num1 /= num2;
                    snprintf(result, 10, "%d", num1);
                break;    
                default:
                    strcpy(result, "Unsupported Operator");
                    break;
                }
            
REPLY:
                // Send reply to client
                printf("Sending Reply: %s\n", result);
                send(socket_client, result, 128, 0);
            } // while 1
        } // if
    } //while(1)
}



int main(int argc, char **argv) {

    int max_socket_client = 10;
    int num_server_session = 1;
    char c;
    while ((c = getopt (argc, argv, "hn:c:")) != -1) {
        switch (c)
        {
            case 'n':
                num_server_session = atoi(optarg);
                break;
            case 'c':
                max_socket_client = atoi(optarg);
                break;
            case 'h':
            case '?':
                printf("Usage: ./server -n <number of server intance> -c <number of client to support>\n\n");
                printf("For case 1:\n ./server -n 1 -c 1\n");
                printf("Here only single instance of server is supported\n");
                printf("and only single connection of client is supported\n\n");
                
                printf("For case 2:\n ./server -n 10 -c 10\n");
                printf("Here only multiple instance of server are supported through fork()\n");
                printf("and multiple connections of client are supported\n\n");
                
                printf("For case 3:\n ./server -n 1 -c 10\n");
                printf("Here only single instance of server is supported\n");
                printf("and multiple connections of client are supported\n\n");

                printf("If no value or incorrect value is passed Case 3 is assumed default\n\n");
                exit(0);
            default:
                abort();
        }
    }

    if (num_server_session == 1 && max_socket_client == 1) {
        printf("Case 1 mode is running\n");
    } else if (num_server_session == 10 && max_socket_client == 10) {
        printf("Case 2 mode is running\n");
    } else {
        num_server_session =1;
        max_socket_client = 10;
        printf("Case 3 mode is running\n");        
    }
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Port where server will listen
    char port[8] = "5050\0" ;
    struct addrinfo *bind_address;
    getaddrinfo(0, port, &hints, &bind_address);

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

    // Start listening to the port 5050
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    printf("Listening on port %s\n",port);

    if(num_server_session == 1) {
        processSingleServer(socket_listen, max_socket_client);
    } else {
        processMultiServer(socket_listen, max_socket_client);
    }

    printf("Closing listening socket...\n");
    CloseSocket(socket_listen);

    return 0;
}