############################### COMPILING CODE #######################################

g++ poll_client.c -o poll_client -lpthread
g++ poll_host.c -o poll_host
g++ poll_server.c -o server -lpthread

################################ OUTPUT OF CODE #######################################

Polling Server:

(base) laffy@laffy-fed:~/workspace/bits_pilani/netw_prog/ec1/ex1/Q4$ ./server 
Listening host on port 5050
Listening Clients on port 5150
Connected with client 127.0.0.1 with socket 13
Connected with client 127.0.0.1 with socket 17
Connected with host with socket 18 clients #2
nc 2
Number of connected client: 2
Query received from 18: are you listening to me 1 -> yes 0 -> no
Sending Query: are you listening to me 1 -> yes 0 -> no

Sending Query: are you listening to me 1 -> yes 0 -> no

Poll Started
Time remaining: 54
Response received from 17: 1
Time remaining: 50
Response received from 13: 1
Time remaining:  0
Poll ended
Sending Poll response are you listening to me 1 -> yes 0 -> no

 Client17 - 1
 Client13 - 1
Query received from 18: shall we start 1 -> yes, 0 -> no
Sending Query: shall we start 1 -> yes, 0 -> no

Sending Query: shall we start 1 -> yes, 0 -> no

Poll Started
Time remaining: 56
Response received from 17: 1
Time remaining: 53
Response received from 13: 1
Time remaining:  0
Poll ended
Sending Poll response shall we start 1 -> yes, 0 -> no

 Client17 - 1
 Client13 - 1
^C


##################################################################################
Polling host:

laffy@laffy-fed:~/workspace/bits_pilani/netw_prog/ec1/ex1/Q4$ ./poll_host 127.0.0.1 5050
Number of connected client: 2 
Time to start a poll, Please enter a poll question:are you listening to me 1 -> yes 0 -> no
Poll Ended (69 bytes)=>
 Clients response for query: are you listening to me 1 -> yes 0 -> no

 Client17 - 1
 Client13 - 1 
Time to start a poll, Please enter a poll question:shall we start 1 -> yes, 0 -> no
Poll Ended (69 bytes)=>
 Clients response for query: shall we start 1 -> yes, 0 -> no

 Client17 - 1
 Client13 - 1 
Time to start a poll, Please enter a poll question:
Connection closed by peer.
Closing socket...
Finished.


######################################################################################

Polling client 1

laffy@laffy-fed:~/workspace/bits_pilani/netw_prog/ec1/ex1/Q4$ ./poll_client 127.0.0.1 5150
Wait for Poll start from polling server
Time idle:  20  
New Poll started :
 are you listening to me 1 -> yes 0 -> no
 
Please enter your response:
Time idle:   6  1
your response 1

Time idle:  54  
Poll results arrived :
 are you listening to me 1 -> yes 0 -> no

 Client17 - 1
 Client13 - 1 
Time idle:  24  
New Poll started :
 shall we start 1 -> yes, 0 -> no
 
Please enter your response:
Time idle:   4  1
your response 1

Time idle:  56  
Poll results arrived :
 shall we start 1 -> yes, 0 -> no

 Client17 - 1
 Client13 - 1 
Time idle:  28  Connection closed by peer.

#################################################################################################

Polling client #2

laffy@laffy-fed:~/workspace/bits_pilani/netw_prog/ec1/ex1/Q4$ ./poll_client 127.0.0.1 5150
Wait for Poll start from polling server
Time idle:  22  
New Poll started :
 are you listening to me 1 -> yes 0 -> no
 
Please enter your response:
Time idle:  11  1
your response 1

Time idle:  49  
Poll results arrived :
 are you listening to me 1 -> yes 0 -> no

 Client17 - 1
 Client13 - 1 
Time idle:  25  
New Poll started :
 shall we start 1 -> yes, 0 -> no
 
Please enter your response:
Time idle:   7  1
your response 1

Time idle:  53  
Poll results arrived :
 shall we start 1 -> yes, 0 -> no

 Client17 - 1
 Client13 - 1 
Time idle:  28  Connection closed by peer.


