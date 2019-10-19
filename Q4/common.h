#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int IsValidSocket(int soc) {
     return soc >= 0;
}
 
void CloseSocket(int soc) {
    close(soc);
}

typedef int SOCKET;
