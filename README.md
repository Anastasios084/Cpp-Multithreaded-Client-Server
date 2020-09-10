# Cpp-Multithreaded-Client-Server
A simple c++ multithreaded tcp server-client programm with custom communication protocol.

In this project you will find many data structures built from scratch and used to organize data in a way to answer queries from a client as fast as possible.
There are 3 main programs. The server, the client and the master.

The client is a programm that reads queries from a file and sends these queries to a server and waits for an answer.
The server is a programm that recieves these queries from a client, communicates with the children of the master to get the answers to these queries and sends them back to the client.
The master is a programm that is specifically made to create children/subprocesses. These children initialize the data appropriately, built the answer to a query and send it back to the server.

These 3 programs can be executed from 3 different places/computers or local of course.
