#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h> 

int fd_parent, fd_child; 

// when ctrl+c is pressed close all open file descriptor
void ctrl_C_Handler(int signum) {
    pid_t pid = getpid();
    printf("\nTime to terminate said pid %d\n",pid);
    if(fd_child > 0) {
        close(fd_child);
        fd_child = -1;
    }
    if(fd_parent > 0) {
        close(fd_parent);
        fd_parent = -1;
    }
    exit(signum);
}

int main() {
    char str[128];
  
    //create a temp FIFO file at /tmp 
    char * ec1_fifo = "/tmp/ec1fifo"; 
  
    // Create the named file(FIFO) 
    mkfifo(ec1_fifo, 0666); 
  
    signal(SIGINT, ctrl_C_Handler);
    // fork parent to create a child process
    pid_t pID = fork();
    
    if (pID > 0)  // parent
    {
        
        pid_t parent_pid = getpid();
        printf("Welcome parent with pid: %d\n", parent_pid); 

        // Parent process open fifo file for write 
        fd_parent = open(ec1_fifo, O_WRONLY); 
        if(fd_parent == -1) {
            printf("fifo open failed");
            return 1;
        }
        while (1) 
        { 
            printf("\nEnter What you want to tell the child process\n"); 
            // Get input from stdin 
            fgets(str, 128, stdin); 
  
            // Write the input into FIFO buffer
            write(fd_parent, str, strlen(str)+1); 
  
        } 
         close(fd_parent); 
         fd_parent = -1;

    }
    else if (pID < 0)    // failed to fork
    {
        printf("\nForking failed\n");
        exit(1);
    }
    else  // child
    {
        char str1[128]; 
        pid_t child_pid = getpid(); 
        printf("Welcome child with pid: %d\n", child_pid); 

        // Open Fifo file to read data from parent side 
        fd_child = open(ec1_fifo,O_RDONLY); 
        if(fd_child == -1) {
            printf("fifo open failed");
            return 2;
        }

        while (1) 
        { 
            // get string from parent
            read(fd_child, str1, 128); 
  
            // put string from parent to stdout 
            printf("Parent said: %s\n", str1); 
        } 
        close(fd_child); 
        fd_child = -1;
    }
 
    return 0;
}