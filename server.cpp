#include <unistd.h> 
#include <fstream>
#include <iostream>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <string>
#include <sys/select.h>
#include "auxilliaryShared.h"
#include "Bucket.h"
#include <sstream>
#include "cyclicBuffer.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>

using namespace std;


int bufferSize = SIZE;

int Bucket::size = 0 ;
int numWorkers = 999999;

void test(){
    cyclicBuffer* buffer = new cyclicBuffer(5);
    for(int i = 0; i < 5; i++){
        buffer->insert(i);
    }
    for(int i = 0; i < 5; i++){
        cout << buffer->pop() << endl;
    }
    cout << buffer->pop() << endl;
    for(int i = 2; i < 5; i++){
        buffer->insert(i);
    }
    cout << buffer->pop() << endl;
    for(int i = 0; i < 3; i++){
        buffer->insert(i);
    }
    cout << "OKAY" << endl;
    buffer->insert(69);
    // for(int i = 10; i < 15; i++){
    //    cout << buffer->pop() << endl;
    // }
    
}

int posInWorkers = 0;
int* workerfds;
cyclicBuffer* bufferFD;
string** ip_Port;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexWorker = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexServer = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutexOut = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t full = PTHREAD_COND_INITIALIZER;



string output = "";

int answered = 0;

int flagEND = 0;
void endingHandler(int signum){
    flagEND = 1;
}

void* thread_func(void* arg){
    while(1){
        //cout << "I AM A THREAD!" << endl;
        //COMPETE FOR FD!
        pthread_mutex_lock(&mutex);
        while(bufferFD->empty()){
            pthread_cond_wait(&empty, &mutex);
            if(flagEND == 1)
                break;
        }
        if(flagEND == 1){
            //cout << "OPA MPRO" << endl;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&empty);
            break;
        }
        int fd = bufferFD->pop();

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        //check for statistics and port.

        pthread_mutex_lock(&mutexWorker);

        if(posInWorkers < numWorkers){
            char buffer[bufferSize];

            string message = ""; // initialize it
            while(readLine(&message, fd, buffer, bufferSize));
            cout << message << endl;

            string port;
            while(readLine(&port, fd, buffer, bufferSize));
            *ip_Port[posInWorkers] = *ip_Port[posInWorkers] + "/" + port;

            posInWorkers++;

            pthread_mutex_unlock(&mutexWorker);
            
            char temp[bufferSize];
            while(read(fd, temp, bufferSize)!=0);
            if(close(fd) == -1){
                cout << "PROBLEM" << endl;
            }

            continue;
        }else{
            pthread_mutex_unlock(&mutexWorker);
        }

        //from now on only query fds!!!!!!


        string ip;
        int port;


        fd_set rfds;
        struct timeval tv;
        string first_word;

        string *answer = new string();
        string message;
        string end = "^";
        *answer = "";
        //changing and getting "input" from client
        //getline(cin, *answer);
        char buffer[bufferSize];

        //bzero(buffer, bufferSize);
        while(readLine(answer, fd, buffer, bufferSize));
        //new connection (CLIENT)
        if(*answer == ""){
            delete(answer);
            continue;
        }
        string current_output;
        current_output = *answer + "\n";
        //cout << "QUERY -> " << *answer << endl;
        if (answer == NULL)
        {
            cout << "Input is NULL ~ returning..." << endl;
            return NULL;
        }

        string word;
        stringstream iss(*answer);
        iss >> word;
        first_word = word;

        message = "";
        //sending the answer to the workers and expect an answer
        for(int i = 0; i < numWorkers; i++){
             //from here
            getIp_Port(ip_Port[i], &ip, &port);
            //cout << "CONNECTING TO -> " << ip << ":" << port << endl;
            int sock = 0, valread; 
            struct sockaddr_in serv_addr; 

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
            { 
                printf("\n Socket creation error \n"); 
                return NULL; 
            } 

            serv_addr.sin_family = AF_INET; 
            serv_addr.sin_port = htons(port); 
            
            // Convert IPv4 and IPv6 addresses from text to binary form 
            if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0) 
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


            writeLine(answer,sock,buffer,bufferSize);
            writeLine(&end,sock,buffer, bufferSize);

            string messageFromChild = "";
            //cout << "Waiting for answer from worker number -> " << i << endl; 
            while(readLine(&messageFromChild, sock, buffer, bufferSize));
            message = message + messageFromChild;
            //cout << "Got out minima -> " << message << endl;
            close(sock);

        }


        if(first_word == "/diseaseFrequency"){
            int RESULTS = 0;
            //cout << message;
            istringstream stream(message);
            string line;
            while(getline(stream,line)){
                RESULTS = atoi(line.c_str()) + RESULTS;
            }

            message = to_string(RESULTS);
            //cout << message  << endl;
            current_output = current_output + message;
            writeLine(&message, fd, buffer, bufferSize);
            writeLine(&end, fd, buffer, bufferSize);
        }else{
            //cout << message  << endl;
            current_output = current_output + message;
            writeLine(&message, fd, buffer, bufferSize);
            writeLine(&end, fd, buffer, bufferSize);
        }
        delete(answer);
        answered++;
        pthread_mutex_lock(&mutexOut);
        cout << current_output << endl;
        pthread_mutex_unlock(&mutexOut);
       
        //cout << "THIS IS HOW MANY QUERIES ARE ANSWERED -> " << answered << endl;
        char temp[bufferSize];
        while(read(fd, temp, bufferSize)!=0);
        if(close(fd) == -1){
            cout << "PROBLEM" << endl;
        }
        //shutdown(fd,SHUT_RDWR);

    }
    return NULL;
}



#define SMALLEST_STACKSZ PTHREAD_STACK_MIN
#define SMALL_STACK (24*1024)




int main(int argc, char const **argv) 
{   
    struct sigaction ending{};
    ending.sa_flags = 0;
    ending.sa_handler = endingHandler;
    sigaction(SIGINT, &ending, NULL);
    sigaction(SIGQUIT, &ending, NULL);
    sigfillset(&ending.sa_mask);

    //test();
    Bucket::size = 256;
    int numThreads,queryPortNum, statisticsPortNum, bufferSizeFD;
    
    for (int i = 1; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-q"))
        {
            queryPortNum = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-w"))
        {
            numThreads = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-s"))
        {
            statisticsPortNum = atoi(argv[i + 1]);
        }else if(!strcmp(argv[i], "-b"))
        {
            bufferSizeFD = atoi(argv[i+1]);
        }
    }

    bufferFD = new cyclicBuffer(bufferSizeFD);


    cout << "This is the query port -> " << queryPortNum << endl;
    cout << "This is the Buffer Size -> " << bufferSize << endl;

    // pthread_attr_t attr;
    // pthread_attr_init(&attr);
    // pthread_attr_setstacksize(&attr, SMALL_STACK);
    // pthread_attr_setdetachstate(&attr, 1);
     //int workers_socket = createConnection(statisticsPortNum); //need to fix
    pthread_t threadPool[numThreads], threadPrint;
    for(int i = 0; i < numThreads; i++){
        pthread_create(&threadPool[i], NULL, thread_func, NULL);
    }


//NEW CONNECTION
    //from here

    int server_fd2, statistics_socket, valread2; 
    struct sockaddr_in address2; 
    int opt2 = 1; 
    int addrlen2 = sizeof(address2); 
    if ((server_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    // MAY NEED TO CHANGE
    if (setsockopt(server_fd2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                &opt2, sizeof(opt2))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    address2.sin_family = AF_INET; 
    address2.sin_addr.s_addr = INADDR_ANY; 
    address2.sin_port = htons( statisticsPortNum ); 

    if (bind(server_fd2, (struct sockaddr *)&address2,  
                                sizeof(address2))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    if (listen(server_fd2, 9999) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((statistics_socket = accept(server_fd2, (struct sockaddr *)&address2,  
                    (socklen_t*)&addrlen2))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    //until here it's the initialization of the connection
    cout << "Connected to the worker..." << endl;


    char ipstr[INET6_ADDRSTRLEN];
    void* addr = &address2.sin_addr;
    inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
     
    //END OF WORKER CONNECTIONS

    string message = "";
    string end = "^";
    char buffer[bufferSize];
    
    //getting the NUMBER OF WORKERS
    while(readLine(&message, statistics_socket, buffer, bufferSize));
    numWorkers = atoi(message.c_str());
    //initilizing the array to hold the ip/port of the workers
    ip_Port = (string**)malloc(sizeof(string**) * numWorkers);
    for(int i = 0; i < numWorkers; i++){
        ip_Port[i] = new string();
    }
    ip_Port[0]->assign(ipstr);
    // inet_pton(AF_INET, servIP.c_str(), &serv_addr.sin_addr);
    
    workerfds = (int*)malloc(sizeof(int)*numWorkers);
    workerfds[0] = statistics_socket;                   //statistics socket is the fd between worker and server NOT th elistening socket!!!!

    cout << "NUM OF WORKERS -> " << numWorkers << endl;
    for(int i = 1; i < numWorkers; i++){
        if ((statistics_socket = accept(server_fd2, (struct sockaddr *)&address2,  
                        (socklen_t*)&addrlen2))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }

        char ipstrTEMP[INET6_ADDRSTRLEN];
        void* addrTEMP = &address2.sin_addr;
        inet_ntop(AF_INET, addr, ipstrTEMP, sizeof(ipstrTEMP)); // getting the ip from binary to char

        ip_Port[i]->assign(ipstrTEMP); 

        workerfds[i] = statistics_socket;
        while(readLine(&message, statistics_socket, buffer, bufferSize));
        cout << "CONNECTION ACCEPTED" << endl;
        
        pthread_mutex_lock(&mutex);
        while(bufferFD->full()){
            pthread_cond_wait(&empty, &mutex);
        }
        bufferFD->insert(workerfds[i-1]);       //wait for full condition
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mutex);

    }
    pthread_mutex_lock(&mutex);
    while(bufferFD->full()){
        pthread_cond_wait(&full, &mutex);
    }
    bufferFD->insert(workerfds[numWorkers-1]);
    pthread_cond_broadcast(&empty);
    pthread_mutex_unlock(&mutex);

    ////////////////////////////////////////////////////////////// totally initialized with master, ready to connect and answer
    
    
    //from here
    int server_fd, query_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    // MAY NEED TO CHANGE
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( queryPortNum ); 
    
    if (bind(server_fd, (struct sockaddr *)&address,  
                                sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 9999) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((query_socket = accept(server_fd, (struct sockaddr *)&address,  
                    (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    //until here it's the initialization of the connection
    cout << "Connected to the client..." << endl;
        //END OF CREATION OF CONNECTIONS
    //////////////////////////////////////////////////////////////////////////
    
    int new_connection; //work forever
    while((new_connection = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen)) >= 0){
        pthread_mutex_lock(&mutex2);
        while(bufferFD->full()){
            pthread_cond_wait(&full,&mutex2);
        }
        
        bufferFD->insert(new_connection);
        pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&mutex2);

        //cout << "NEW CONNECTION!" << endl;
    }
    if(flagEND == 1){
        pthread_mutex_unlock(&mutex);

        delete(bufferFD);
        for(int i = 0; i < numWorkers; i++){
            delete(ip_Port[i]);
        }
        free(ip_Port);
        free(workerfds);        

        pthread_cond_broadcast(&empty);
        for(int i = 0; i < numThreads; i++){
            pthread_join(threadPool[i], NULL);
        }
    }
    return 0; 
} 
