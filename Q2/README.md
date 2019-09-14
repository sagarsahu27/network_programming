############ To compile code ########################

gcc server.c -o server
gcc client.c -o client


################# CLIENT SIDE OUTPUT #################################

######### Tried to connect Client 11 

./client 127.0.0.1 5050
Connection closed by peer.
Closing socket...
Finished.


############# Running various command through client

./client 127.0.0.1 5050
Please enter the command:ls | grep -i cl
Server Replied (16 bytes):
 client
client.c



./client 127.0.0.1 5050
Please enter the command:ls -alh
Server Replied (406 bytes):
 total 56K
drwxr-xr-x 2 laffy laffy 4.0K Sep 14 16:47 .
drwxrwxr-x 6 laffy laffy 4.0K Sep 14 13:36 ..
-rw-rw-r-- 1 laffy laffy    0 Sep 14 15:28 a.out
-rwxrwxr-x 1 laffy laffy  14K Sep 14 16:37 client
-rw-rw-r-- 1 laffy laffy 2.3K Sep 14 16:30 client.c
-rw-rw-r-- 1 laffy laffy  334 Sep 14 14:41 common.h
-rwxrwxr-x 1 laffy laffy  14K Sep 14 16:47 server
-rw-rw-r-- 1 laffy laffy 4.7K Sep 14 16:47 server.c
 



./client 127.0.0.1 5050
Please enter the command:ls -a
Server Replied (52 bytes):
 .
..
a.out
client
client.c
common.h
server
server.c
 


./client 127.0.0.1 5050
Please enter the command:ps
Server Replied (142 bytes):
   PID TTY          TIME CMD
 6871 pts/1    00:00:00 bash
14453 pts/1    00:00:00 server
14501 pts/1    00:00:00 sh
14502 pts/1    00:00:00 ps



./client 127.0.0.1 5050
Please enter the command:pwd
Server Replied (55 bytes):
 /home/laffy/workspace/bits_pilani/netw_prog/ec1/ex1/Q2


 

./client 127.0.0.1 5050
Please enter the command:ls
Server Replied (47 bytes):
 a.out
client
client.c
common.h
server
server.c



############## SERVER SIDE OUTPUT #############################

laffy@laffy-fed:~/workspace/bits_pilani/netw_prog/ec1/ex1/Q2$ ./server 
Listening on port 5050
Connected with client 127.0.0.1 with socket 12
Connected with client 127.0.0.1 with socket 13
Connected with client 127.0.0.1 with socket 17
Connected with client 127.0.0.1 with socket 18
Connected with client 127.0.0.1 with socket 19
Connected with client 127.0.0.1 with socket 20
Connected with client 127.0.0.1 with socket 22
Connected with client 127.0.0.1 with socket 23
Connected with client 127.0.0.1 with socket 24
Connected with client 127.0.0.1 with socket 25
Maximum client capacity is reached !!!
Command received from 25: ls
Sending Reply: a.out
client
client.c
common.h
server
server.c

Command received from 24: pwd
Sending Reply: /home/laffy/workspace/bits_pilani/netw_prog/ec1/ex1/Q2

Command received from 23: ps
Sending Reply:   PID TTY          TIME CMD
 6871 pts/1    00:00:00 bash
14453 pts/1    00:00:00 server
14501 pts/1    00:00:00 sh
14502 pts/1    00:00:00 ps

Command received from 22: ls -a
Sending Reply: .
..
a.out
client
client.c
common.h
server
server.c

Command received from 20: ls -alh
Sending Reply: total 56K
drwxr-xr-x 2 laffy laffy 4.0K Sep 14 16:47 .
drwxrwxr-x 6 laffy laffy 4.0K Sep 14 13:36 ..
-rw-rw-r-- 1 laffy laffy    0 Sep 14 15:28 a.out
-rwxrwxr-x 1 laffy laffy  14K Sep 14 16:37 client
-rw-rw-r-- 1 laffy laffy 2.3K Sep 14 16:30 client.c
-rw-rw-r-- 1 laffy laffy  334 Sep 14 14:41 common.h
-rwxrwxr-x 1 laffy laffy  14K Sep 14 16:47 server
-rw-rw-r-- 1 laffy laffy 4.7K Sep 14 16:47 server.c

Command received from 19: ls | grep -i cl
Sending Reply: client
client.c


