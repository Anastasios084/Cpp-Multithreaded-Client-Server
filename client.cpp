#include <iostream>
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <string>
#include "auxilliaryShared.h"
#include "Bucket.h"
#include "clientAuxilliary.h"
#include <pthread.h>

using namespace std;


int Bucket::size = 0 ;
string** queries;
int numOfQueries, servPort;
string servIP;
int bufferSize = SIZE;
int pos = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexOutput = PTHREAD_MUTEX_INITIALIZER;

int answered = 0;

void* thread_func(void* arg){
   
    string message;
    string end = "^";
    

    do{
        string query;
        pthread_mutex_lock(&mutex);
        query.assign(*queries[pos]);
        pos++;
        pthread_mutex_unlock(&mutex);
        if(pos > numOfQueries){
            break;
        }
            //from here
        int sock = 0, valread; 
        struct sockaddr_in serv_addr; 

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        { 
            printf("\n Socket creation error \n"); 
            return NULL; 
        } 

        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(servPort); 
        
        // Convert IPv4 and IPv6 addresses from text to binary form 
        if(inet_pton(AF_INET, servIP.c_str(), &serv_addr.sin_addr)<=0) 
        { 
            printf("\nInvalid address/ Address not supported \n"); 
            return NULL; 
        } 

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
            printf("\nConnection Failed \n"); 
            return NULL; 
        } 
        //until here it's the initialization of the socket connection
        char buffer[bufferSize];
        //cout << query << endl;
        writeLine(&query, sock, buffer, bufferSize);
        writeLine(&end, sock, buffer, bufferSize);

        message = "";
        //may need to clean buffer
        while(readLine(&message,sock, buffer, bufferSize));
        if(message == "")
            message = "Data not found\n";

        pthread_mutex_lock(&mutexOutput);
        cout << query << "\n" << message << endl;
        pthread_mutex_unlock(&mutexOutput);
        //answered++;
        close(sock);
    }while(pos < numOfQueries);
    //cout << "QUERIES ANSWERED -> " << answered << endl;
    
    return NULL;
}

int main(int argc, char const *argv[]) 
{ 

    Bucket::size = 256;
    string message = "";
    string end = "^";

     //initializing from argv

    string queryFile;
    int numThreads;
    
    for (int i = 1; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-q"))
        {
            queryFile = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-w"))
        {
            numThreads = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-sp"))
        {
            servPort = atoi(argv[i + 1]);
        }else if(!strcmp(argv[i], "-sip"))
        {
            servIP = argv[i+1];
        }
    }

    pthread_t threadPool[numThreads];
 

    int sock = 0, valread; 
	struct sockaddr_in serv_addr; 

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(servPort); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, servIP.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 


    //cout << queryFile << numThreads << servPort << servIP << endl;
    //opening and reading queryFile
    char buffer[bufferSize];
    writeLine(&end, sock, buffer, bufferSize);
    if(processQueries(queryFile, &queries, &numOfQueries)){
        cout << "ERROR OPENING QUERY FILE ~ exiting..." << endl;
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < numThreads; i++){
        pthread_create(&threadPool[i], NULL, thread_func, NULL);
    }

    
    for(int i = 0; i < numThreads; i++){
        pthread_join(threadPool[i], NULL);
    }
    
    for(int i = 0; i < numOfQueries; i++){
        delete(queries[i]);
    }
    free(queries);


	return 0; 
} 
