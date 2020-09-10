#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <string>
#include <string.h>
#include <fstream>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
#include "auxilliaryShared.h"
#include "auxilliaryWorker.h"
#include "AVLtree.h"
#include "Auxilliary.h"
#include "Record.h"
#include "Bucket.h"
#include "HashTable.h"
#include "Heap.h"
#include <wait.h>

using namespace std;
int Bucket::size = 0 ;

 char* READnamedPipe; // PARENT WRITES -> CHILD READS
 char* WRITEnamedPipe; // CHILD WRITES -> PARENT READS
 int bufferSize; //bufferSize
 char* input_dir; //This is where the countries/files are
 char* buffer;

int counterReceived = 0;
int counterAnswered = 0;
int countriesNum;

int flagNewEntry = 0;
int flagIntOrQuit = 0;
int flagEND = 0;
int flagUSR2 = 0;

string name;
int READfd, WRITEfd;


    //creating an array of countries from the received message
string** countries;

void catcher(int signum) {
    flagIntOrQuit = 1;
    //cout << "MADE IT 1" << endl;
}

void newEntry(int signum, siginfo_t *siginfo, void* context) {
    flagNewEntry = 1;
}

//not used
void USR2(int signum) {
    flagUSR2 = 1;
}

int main(int argc, char **argv){
    string end = "^";
    //not used
    struct sigaction usr2{};
    usr2.sa_flags = 0;
    usr2.sa_handler = USR2;
    sigaction(SIGUSR2, &usr2, NULL);

    //sigaction for SIGINT or SIGQUIT
    struct sigaction sigact{};
    sigact.sa_flags = 0;
    sigact.sa_handler = catcher;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);
    sigfillset(&sigact.sa_mask);

    //sigaction for SUGUSR1
    struct sigaction sigactUSER{};
    sigactUSER.sa_flags = 0;
    sigactUSER.sa_sigaction = newEntry;
    sigactUSER.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sigactUSER, NULL);
    sigfillset(&sigactUSER.sa_mask);

    sigprocmask(SIG_BLOCK, &sigact.sa_mask, NULL);


    int serverPort, numWorkers;
    string serverIP;

    Bucket::size = 256;
    READnamedPipe = argv[1]; // PARENT WRITES -> CHILD READS
    WRITEnamedPipe = argv[2]; // CHILD WRITES -> PARENT READS
    bufferSize = atoi(argv[3]); //bufferSize
    input_dir = argv[4];
    serverPort = atoi(argv[5]);
    serverIP = argv[6];
    numWorkers = atoi(argv[7]);
    

    name = "log_file." + to_string(getpid());
    
    buffer = (char*)malloc(sizeof(char)*bufferSize); //Creating buffer size of bufferSize


    mkfifo(READnamedPipe, 0777);
    READfd = open(READnamedPipe, O_RDONLY);

    mkfifo(WRITEnamedPipe, 0777);
    WRITEfd = open(WRITEnamedPipe, O_WRONLY);


    string* message = new string();
    *message = "";

    //cout << READnamedPipe << " - " << WRITEnamedPipe << " - " << bufferSize << " - " << input_dir << endl;
 


    //reading the countries in one big line
    while(readLine(message, READfd, buffer, bufferSize));
    //cout << *message << endl;
    
    //not used
    if(*message == "000" ){
        cout << "Closing.... received 0 countries" << endl;
        cout << "Closing reading Pipe..." << endl;
        close(READfd);
        cout << "Closing writing Pipe..." << endl;
        close(WRITEfd);
        delete(message);
        int status;
        waitpid(getppid(),&status, 0);
        exit(1);//?
        return 0;
    }
    ///////////////////

    //creating an array of countries from the received message
    countries = initializeCountries(message, &countriesNum);

    bufferSize = SIZE;
    free(buffer);
    buffer = (char*)malloc(bufferSize);

    //creating connection
    int sock = 0, valread; 
	struct sockaddr_in serv_addr; 

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(serverPort); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
    //created connection

    READfd = sock;
    WRITEfd = sock;

    *message = to_string(numWorkers);
    //cout << *message << endl;
    writeLine(message, WRITEfd, buffer, bufferSize);
    writeLine(&end, WRITEfd, buffer, bufferSize);
    

    //this holds the latest date files for each country in case of SIGUSR1
    string** latestDatesPerCountry = (string**)malloc(sizeof(string*)*countriesNum); 
    for(int i = 0; i < countriesNum; i++){
        latestDatesPerCountry[i] = new string();
    }

    HashTable* diseaseHashTable = new HashTable(256);
    HashTable* countryHashTable = new HashTable(256);
                                        //initializing database, creating stats and sending them to the parent
    LinkedList<Record*>* countryRecs = openFilesFromCountryDirsSorted(countries, countriesNum, latestDatesPerCountry, input_dir, &diseaseHashTable, &countryHashTable,WRITEfd,buffer,bufferSize);
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //ofstream myfile(name);

    //HERE IS THE QUERY PART
    //from here                         CREATING LISTENING SOCKET!!!!!!!!!!!!!!!!!!!
    int server_fd_LISTENING, query_socket_LISTENING, valread_LISTEING; 
    struct sockaddr_in address_LISTENING; 
    int opt_LISTENING = 1; 
    int addrlen_LISTENING = sizeof(address_LISTENING); 
    if ((server_fd_LISTENING = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    // MAY NEED TO CHANGE
    if (setsockopt(server_fd_LISTENING, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                &opt_LISTENING, sizeof(opt_LISTENING))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    address_LISTENING.sin_family = AF_INET; 
    address_LISTENING.sin_addr.s_addr = INADDR_ANY; 
    address_LISTENING.sin_port = htons( 0 ); 

    //cout << "THIS IS BEFORE BIND -> "<< ntohs(address_LISTENING.sin_port) << endl;
    if (bind(server_fd_LISTENING, (struct sockaddr *)&address_LISTENING,  
                                sizeof(address_LISTENING))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 


    if (listen(server_fd_LISTENING, 9999) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    socklen_t len = sizeof(address_LISTENING);
    if (getsockname(server_fd_LISTENING, (struct sockaddr *)&address_LISTENING, &len) == -1)
        perror("getsockname");

    //cout << "THIS IS AFTER BIND -> " << ntohs(address_LISTENING.sin_port) << endl;


    string port = to_string(ntohs(address_LISTENING.sin_port));
    //cout << "STRING PORT -> " << port << endl;

    writeLine(&port,WRITEfd,buffer,bufferSize);
    writeLine(&end,WRITEfd,buffer,bufferSize);

    close(sock);







    sigprocmask(SIG_UNBLOCK, &sigact.sa_mask, NULL);




    //LISTENING
    cout << "LISTENING... type /exit if you want to EXIT..." << endl;
    while((query_socket_LISTENING = accept(server_fd_LISTENING, (struct sockaddr *)&address_LISTENING,(socklen_t*)&addrlen_LISTENING)) >= 0){
        //cout << "GOT A NEW QUERY!!!" << endl;
        READfd = query_socket_LISTENING;
        WRITEfd = query_socket_LISTENING;
        string *answer = new string();
        *message = "";
        //unblocking all the signals while reading
        while(readLine(answer,READfd, buffer, bufferSize));

        //blocking all the signals after reading from parent, "can't" be stopped during query
        sigprocmask(SIG_BLOCK, &sigact.sa_mask, NULL);


        if(flagNewEntry == 1){
            flagNewEntry = 0;
            updateRecords(countries, countriesNum, latestDatesPerCountry, input_dir, countryRecs, &diseaseHashTable, &countryHashTable, WRITEfd, buffer, bufferSize);
            //cout << "UPDATED RECORDS" << endl;

            delete(answer);
            kill(getppid(), SIGUSR1);
            continue;
        }

        //not used
        if(flagUSR2 == 1){
            cout << "BROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << endl;
            *answer = "/exit";
        }

        //RECEIVED SIGINT OR SIGQUIT
        if (flagIntOrQuit == 1){
            ofstream myfile(name);

            for(int i = 0; i < countriesNum; i++){
                myfile << *countries[i] << endl;
            }
            myfile << "TOTAL " << counterReceived << endl;
            myfile << "SUCCESS " << counterAnswered << endl;
            myfile << "FAIL " << counterReceived-counterAnswered;
            cout << "CREATED FILE" << endl;
            myfile.close();

            flagIntOrQuit = 0;
            *answer = "/exit";
            kill(getppid(), SIGCHLD);

        }

        if (answer == NULL)
        {
            cout << "Input is NULL ~ returning..." << endl;
            return -1;
        }
        string word;
        stringstream iss(*answer);
        iss >> word;
        //cout << "THIS IS THE RECEIVED MESSAGE: " << *answer << endl;
        counterReceived++;

        if (word == "/diseaseFrequency")
        {
            //cout << "I GOTCHU FAM" << endl;
            string* data[4];
            data[0] = NULL;
            data[1] = NULL;
            data[2] = NULL;
            data[3] = NULL;
            int counter = 0;
            while (iss >> word)
            {
                //cout << word << endl;
                data[counter] = new string();
                *data[counter] = word;
                counter++;
            }
            int numPatients = 0;
            if (counter == 3)
            {
                numPatients = DiseaseFrequency(diseaseHashTable, *data[0], data[3], *data[1], *data[2]);
                counterAnswered++;
            }
            else if (counter == 4)
            {
                numPatients = DiseaseFrequency(diseaseHashTable, *data[0], data[3], *data[1], *data[2]);
                counterAnswered++;

            }
            for(int i = 0; i < 4; i++){
                if(data[i] != NULL){
                    delete(data[i]);
                }
            }

            *message = to_string(numPatients) + "\n";
            writeLine(message, WRITEfd, buffer, bufferSize);
            writeLine(&end, WRITEfd, buffer, bufferSize);
        }else if(word == "/listCountries"){
            *message = to_string(getpid()) + "\n";
            for(int i = 0; i < countriesNum; i++){
                *message = *message + *countries[i] + "\n";
            }
            *message = *message + "\n";
            writeLine(message, WRITEfd, buffer, bufferSize);
            writeLine(&end, WRITEfd, buffer, bufferSize);
            counterAnswered++;
        }else if(word == "/topk-AgeRanges"){
            string* data[5];
            data[0] = NULL;
            data[1] = NULL;
            data[2] = NULL;
            data[3] = NULL;
            data[4] = NULL;
            int counter = 0;
            *message = "";
            while (iss >> word)
            {
                data[counter] = new string();
                *data[counter] = word;
                counter++;
            }
            //checking if it is the appropriate worker
            int i;
            for(i = 0; i < countriesNum; i++){
                if(*countries[i] == *data[1]){
                    break;
                }
            }
            if(i == countriesNum){
                //cout << "Not the right worker" << endl;
            }else if (counter == 5)
            {
                *message = topkAgeRanges(atoi(data[0]->c_str()), data[1], data[2], data[3],data[4], countryHashTable);
                counterAnswered++;
            }
            else{
                cout <<"error wrong number of arguments" << endl;
            }

            for(int i = 0; i < 5; i++){
                if(data[i] != NULL){
                    delete(data[i]);
                }
            }
            writeLine(message, WRITEfd, buffer, bufferSize);
            writeLine(&end, WRITEfd, buffer, bufferSize);

        }else if (word == "/numPatientAdmissions"){
            string* data[4];
            data[0] = NULL;
            data[1] = NULL;
            data[2] = NULL;
            data[3] = NULL;
            int counter = 0;
            *message = "";
            while (iss >> word)
            {
                //cout << word << endl;
                data[counter] = new string();
                *data[counter] = word;
                counter++;
            }

            if(counter == 4){
                //check if it is appropriate worker
                int i;
                for(i = 0; i < countriesNum; i++){
                    if(*countries[i] == *data[3]){
                        break;
                    }
                }
                if(i == countriesNum){
                    //cout << "Not appropriate worker" << endl;
                }else{
                    *message = numPatientsAdmissions(data[0], data[1], data[2], data[3], diseaseHashTable);
                    writeLine(message, WRITEfd, buffer, bufferSize);
                    counterAnswered++;
                }
            }else{
                for(int i = 0; i < countriesNum; i++){
                    *message = *message + numPatientsAdmissions(data[0], data[1], data[2], countries[i], diseaseHashTable);
                }
                writeLine(message, WRITEfd, buffer, bufferSize);
                counterAnswered++;
            }
            writeLine(&end, WRITEfd, buffer, bufferSize);
            for(int i = 0; i < 4; i++){
                if(data[i] != NULL){
                    delete(data[i]);
                }
            }
        }else if(word == "/searchPatientRecord"){
            iss >> word;
            string recID = word;
            Record* Rec = Find(countryRecs, &recID);
            if(Rec == NULL){
                writeLine(&end, WRITEfd, buffer, bufferSize);
            }else{
                *message = *Rec->GetRecId() + " " + Rec->GetPatientFirstName() + " " + Rec->GetPatientLastName() + " " + to_string(Rec->GetAge()) + " " + Rec->GetEntryDate() + " " + *Rec->GetExitDate() + "\n";
                counterAnswered++;
                writeLine(message, WRITEfd, buffer, bufferSize);
                writeLine(&end, WRITEfd, buffer, bufferSize);
            }
        
        }else if(word == "/numPatientDischarges"){
            string* data[4];
            data[0] = NULL;
            data[1] = NULL;
            data[2] = NULL;
            data[3] = NULL;
            int counter = 0;
            *message = "";
            while (iss >> word)
            {
                data[counter] = new string();
                *data[counter] = word;
                counter++;
            }

            if(counter == 4){
                //check if it is appropriate worker
                int i;
                for(i = 0; i < countriesNum; i++){
                    if(*countries[i] == *data[3]){
                        break;
                    }
                }
                if(i == countriesNum){
                    //cout << "Not appropriate worker" << endl;
                }else{
                    *message = numPatientsDischarges(data[0], data[1], data[2], data[3], diseaseHashTable);
                    writeLine(message, WRITEfd, buffer, bufferSize);
                    counterAnswered++;
                }
            }else{
                for(int i = 0; i < countriesNum; i++){
                    *message = *message + numPatientsDischarges(data[0], data[1], data[2], countries[i], diseaseHashTable);
                }
                counterAnswered++;
                writeLine(message, WRITEfd, buffer, bufferSize);
            }
            writeLine(&end, WRITEfd, buffer, bufferSize);
            for(int i = 0; i < 4; i++){
                if(data[i] != NULL){
                    delete(data[i]);
                }
            }
        }else if(word == "/exit"){
            counterReceived--;
            delete(answer);
            cout << "exiting" << endl;

            char temp[bufferSize];
            while(read(query_socket_LISTENING, temp, bufferSize)!=0);
            if(close(query_socket_LISTENING) == -1){
                cout << "PROBLEM" << endl;
            }

            break;
        }else{
            cout <<"Wrong Input ~ Check your arguments and try again!" << endl;
            writeLine(&end,WRITEfd,buffer, bufferSize);
        }

        
        delete(answer);
        
        char temp[bufferSize];
        while(read(query_socket_LISTENING, temp, bufferSize)!=0);
        if(close(query_socket_LISTENING) == -1){
            cout << "PROBLEM" << endl;
        }
        //close(query_socket_LISTENING);
        //shutdown(query_socket_LISTENING,SHUT_RDWR);
        sigprocmask(SIG_UNBLOCK, &sigact.sa_mask, NULL);

    }

    
    //writeLine(&end, WRITEfd, buffer, bufferSize);

    //freeing up memory
    delete(message);
    for(int i = 0; i < countriesNum; i++){
        delete(latestDatesPerCountry[i]);
        delete(countries[i]);
    }
    free(countries);
    free(latestDatesPerCountry);
    delete(diseaseHashTable);
    delete(countryHashTable);
    delete(countryRecs);
    free(buffer);

    // cout << "Closing reading Pipe..." << endl;
    // close(READfd);
    // cout << "Closing writing Pipe..." << endl;
    // close(WRITEfd);
    exit(1);
    return 0;
}