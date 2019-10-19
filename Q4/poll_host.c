#include "common.h"

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage: ./client ip_address port\n");
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
        return 1;
    }

    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);

    // create socket
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!IsValidSocket(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    // connect to server 
    if (connect(socket_peer,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(peer_address);

    while(1) {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
   
        if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", errno);
            return 1;
        }

        // Receiver data from server
        if (FD_ISSET(socket_peer, &reads)) {
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
            if(strlen(read)) {
              printf("Server Replied (%ld bytes):\n %.*s \n",
                    strlen(read), bytes_received, read);
            }
        } else {
            // Get command from client input
            char read[4096];
            printf("Please enter the command:");
            if (!fgets(read, 4096, stdin)) break;

            // Send data to server
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
        }
    } //end while(1)

    printf("Closing socket...\n");
    CloseSocket(socket_peer);
    printf("Finished.\n");
    return 0;
}
