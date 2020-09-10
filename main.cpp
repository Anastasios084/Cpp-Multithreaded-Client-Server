#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <wait.h>
#include "auxilliaryShared.h"
#include <signal.h>
#include "AVLtree.h"
#include "Auxilliary.h"
#include "Record.h"
#include "Bucket.h"
#include "HashTable.h"
#include "Heap.h"
#include <sys/select.h>

using namespace std;

int* childrenPids;
int numWorkers;
int flagNEWFILE = 0;
int flagIntOrQuit = 0;
int sendersPID;
int flagEND = 0;

void catcher(int signum, siginfo_t *sigin, void* context) {
  sendersPID = (int)sigin->si_pid;
  flagNEWFILE = 1;
}

void catcherChild(int signum, siginfo_t *sigin, void* context) {
  sendersPID = (int)sigin->si_pid;
  flagIntOrQuit = 1;
}

void endingHandler(int signum){
    flagEND = 1;
}


int Bucket::size = 0 ;

int main(int argc, char **argv){
    //this is to count the queries
    int total = 0;
    int success = 0;
    int fail = 0;
    
    Bucket::size = 256;

    ofstream myfile;
    //myfile.open ("test");

    //sigact for SIGINT or SIGQUIT
    struct sigaction ending{};
    ending.sa_flags = 0;
    ending.sa_handler = endingHandler;
    sigaction(SIGINT, &ending, NULL);
    sigaction(SIGQUIT, &ending, NULL);
    sigfillset(&ending.sa_mask);

    //sigact for SIGUSR1
    struct sigaction sigact{};
    sigfillset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = catcher;
    sigaction(SIGUSR1, &sigact, NULL);

    //sigact for SIGCHLD
    struct sigaction sigactChild{};
    sigfillset(&sigactChild.sa_mask);
    sigactChild.sa_flags = SA_SIGINFO;
    sigactChild.sa_sigaction = catcherChild;
    sigaction(SIGCHLD, &sigactChild, NULL);

    sigprocmask(SIG_BLOCK, &sigactChild.sa_mask, NULL);
    
    //program starts from here really
    string input_dir;
    int bufferSize;

    pid_t pid;

    fd_set rfds;
    struct timeval tv;


    int numOfCountries;
    string serverIP;
    int serverPort;

    //initializing from argv
    for (int i = 1; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-i"))
        {
            input_dir = argv[i + 1];
        }
        else if (!strcmp(argv[i], "-w"))
        {
            numWorkers = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-b"))
        {
            bufferSize = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-p"))
        {
            serverPort = atoi(argv[i + 1]);
        }
        else if (!strcmp(argv[i], "-s"))
        {
            serverIP = argv[i + 1];
        }
    }

    //create a string** array with the name of countries and get the size of the array
    string** countries = getCountries(&input_dir,&numOfCountries);
    
    //creating a static array
    string WRITEnamedPipes[numWorkers];
    string READnamedPipes[numWorkers];

    char* buffer = (char*)malloc(sizeof(char)*bufferSize); //Creating buffer size of bufferSize

    childrenPids = (int*)malloc(sizeof(int)*numWorkers);    //creating an array which holds the pids of the children

    //creating arrays for holding the pipes fds
    int WRITEfd[numWorkers];
    int READfd[numWorkers];

    //initializing the pipes for read and write
    initializeNamedPipes(WRITEnamedPipes,READnamedPipes, WRITEfd, READfd,numWorkers);
    int to_be_sent = numWorkers;
    if(numWorkers > numOfCountries){
        to_be_sent = numOfCountries;
    }
    //Opening/creating the workers
    for (int i = 0; i < numWorkers; ++i) {
        //cout << WRITEnamedPipes[i] << " - " << READnamedPipes[i] << endl;
        if ((pid = fork()) < 0) {
            perror("Could not fork, please create clients manually");
        }

        else if (pid == 0) {
            // We're in the child process, start a peer
            execl("./worker", "./worker",WRITEnamedPipes[i].c_str(), READnamedPipes[i].c_str(), to_string(bufferSize).c_str(), input_dir.c_str(),to_string(serverPort).c_str(), serverIP.c_str(),to_string(to_be_sent).c_str(),(const char*)nullptr);
            exit(127);
        }
        childrenPids[i] = pid;
        WRITEfd[i] = open(WRITEnamedPipes[i].c_str(), O_WRONLY);
    }

    //MAKING READ RDONLY AGAIN!!!! 
    for(int i = 0; i < numWorkers; i++){ 
        //cout << childrenPids[i] << endl;
        fcntl(READfd[i], F_SETFL, O_RDONLY);
    }
    //sending a message bigger than the buffer
    string message = "HELLO!";
    string end = "^";
    
    int inputNumWorkers = numWorkers;

    if(numWorkers > numOfCountries){
        // message = "000";
        // cout << "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE ------> " << numOfCountries << "   " << numWorkers << endl;
        // for(int i = numOfCountries; i < numWorkers; i++){
        //     cout << "BRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR" << endl;
        //     writeLine(&message, WRITEfd[i], buffer, bufferSize);
        //     writeLine(&end,WRITEfd[i], buffer, bufferSize);
        //     int status;
        //     waitpid(childrenPids[i],&status, 0);
        //     close(READfd[i]);
        //     //cout << "Closing write FD number: " << i << endl;
        //     close(WRITEfd[i]);
        //     cout << "RIP CHILD -------------------------> " << i << endl;
        // }
        numWorkers = numOfCountries; 
    }

    int j = 0;
    //assigning countries to workers
    for(int i = 0; i < numWorkers; i++){
        if(j == numOfCountries){
            break;
        }

        message = *countries[j] + " ";
        j++;
        writeLine(&message, WRITEfd[i], buffer, bufferSize);
        if(i == numWorkers-1 && j < numOfCountries){
            i = -1;
        }
    }


    //ENDING MESSAGE
    for(int i = 0; i < numWorkers; i++){
        writeLine(&end, WRITEfd[i], buffer, bufferSize);
    }
    string answer;
    while(getline(cin, answer)){
        if(answer == "/exit")
            break;
    }
    for(int i = 0; i < numWorkers; i++){
        kill(childrenPids[i], SIGUSR2);
    }

    free(childrenPids);
    free(buffer);
    for(int i = 0; i < numOfCountries; i++){
        delete(countries[i]);
    }
    free(countries);
    for(int i = 0; i < numWorkers; i++){
        int status;
        //cout << "Closing read FD number: " << i << endl;
        close(READfd[i]);
        //cout << "Closing write FD number: " << i << endl;
        close(WRITEfd[i]);
    }
    cout << "Closed Master and Workers..." << endl;
    return 0;
}