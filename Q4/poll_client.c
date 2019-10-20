#include "common.h"
#include <pthread.h>

int time_out = 0;

void * idle_time_handler(void *i) {
    int *t = (int *)i;
    while(*t<120) {
        sleep(1);
        *t += 1;
        if (*t >= 60) {
            printf("\rTime idle: %3d  ", *t);
            fflush(stdout);           
        }
    } 
    time_out = 1;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage: ./poll_client ip_address 5150\n");
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

    printf("Wait for Poll start from polling server\n");
    int i=0;
    pthread_t timer_thread;
    if(pthread_create(&timer_thread, NULL, idle_time_handler, (void *)&i)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }

    int poll_status = 0;

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
        if (i >= 120) {
            printf("\r Timeout Connection closed due to idle.\n");
            CloseSocket(socket_peer);
            break;
        }
        // Receiver data from server
        if (FD_ISSET(socket_peer, &reads)) {
            i = 0;
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                CloseSocket(socket_peer);
                exit(0);
            }
            if(strlen(read)) {
                if (poll_status) {
                    poll_status = 0;
                    printf("\nPoll results arrived :\n %.*s \n",
                        bytes_received, read);
                } else {
                    poll_status = 1;
                    printf("\nNew Poll started :\n %.*s \n",
                        bytes_received, read);
                }
            }
        } else {
            if (poll_status == 1) {
                char resp[10];
                printf("\rPlease enter your response:\n");
                if (!fgets(resp, 10, stdin)) break;
                printf("\ryour response %s\n", resp);
                int x = atoi(resp);
                x = (x && 1);
                snprintf(resp, 10, "%d", x);
                // Send data to server
                int bytes_sent = send(socket_peer, resp, strlen(resp), 0);
                poll_status = 2;
                i = 0;
            }
        } 
    } //end while(1)

    if(pthread_join(timer_thread, NULL)) {

        fprintf(stderr, "Error joining thread\n");
        return 2;

    }

    printf("Closing socket...\n");
    CloseSocket(socket_peer);
    printf("Finished.\n");
    return 0;
}
