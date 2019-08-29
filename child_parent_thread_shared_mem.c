#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h> 

 int shm_id;
 pid_t ppid;
 #define KEY 2762019

void ctrl_C_Handler(int signum) {
    pid_t pid = getpid();
    printf("\nTime to terminate said pid %d\n",pid);
        // Check for parent pid to detroy shared memory
        if(ppid == pid) {
         // destroy the shared memory 
        shmctl(shm_id,IPC_RMID,NULL); 
        }

    exit(signum);
}


void* child_proc_handler(void *str) {
    char* shared_mem_ptr = (char*) str;
    // fork parent to create a child process
    pid_t pID = fork();
    
    if (pID < 0) // failed to fork
    {
        printf("\nForking failed\n");
        exit(1);
    }
    else if(pID == 0) // child
    {
        pid_t child_pid = getpid(); 
        printf("\nWelcome child with pid: %d\n", child_pid); 
        char temp_str[128] = "";
        while(1) {
            if(strcmp(temp_str, shared_mem_ptr)) {
                printf("\nMessage from parent: %s\n",shared_mem_ptr); 
                strncpy(temp_str, shared_mem_ptr, 128);
            }

        }
    }

}

int main() {
   
    const int max_message_size = 128; 
    shm_id = shmget(KEY, max_message_size, IPC_CREAT | IPC_EXCL| 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    // shmat to attach to shared memory 
    char *str = (char*) shmat(shm_id,0,0); 

    signal(SIGINT, ctrl_C_Handler);
    
    pthread_t child_thread;

    if(pthread_create(&child_thread, NULL, child_proc_handler, str)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }

        pid_t parent_pid = getpid();
        ppid = parent_pid;
        printf("\nWelcome parent with pid: %d\n", parent_pid); 
        printf("\nSetting up shared memory\n");
        sleep(1);
        while(1) {
            printf("\nEnter what you want to send to child:\n");
            fgets(str,max_message_size,stdin); 
        }
         //detach from shared memory  
         shmdt(str); 

    if(pthread_join(child_thread, NULL)) {

        fprintf(stderr, "Error joining thread\n");
        return 2;

    }

    return 0;
}