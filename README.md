# Socket-Chat-Server
A simple socket programming which creates a connection between two terminals on linux.

To build the executable files, type:
$ make all

To set up a server, type:
$ ./server portNumber

To connect a client to the server, type:
$ ./client hostName portNumber

Upto 10 clients can connect to the server. If you want more to connect,
simply change the code in server.c
