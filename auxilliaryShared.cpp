#include "auxilliaryShared.h"
#include <dirent.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fstream>
#include <sstream>
#include <sys/socket.h> 
#include <arpa/inet.h> 

using namespace std;

string** getCountries(string* directoryString, int* size){
    DIR *dir;
    struct dirent *dent;
    const char* directory = directoryString->c_str();
    dir = opendir(directory);
    int counter = 0;
    string allCountries;

    if(dir!=NULL){
        while((dent=readdir(dir))!=NULL){
            if(dent->d_name[0] != '.'){
                counter++;
                allCountries = allCountries + dent->d_name + "\n";
            }
            
        }
    }

    string** countries = (string**)malloc(sizeof(string*)*counter);
    for(int i = 0; i < counter; i++){
        countries[i] = new string();
    }
    for(int i = 0; i < counter; i++){
        *countries[i] = "";
    }
    int i = 0;
    int index = 0;
    while(i < counter){
        if(allCountries[index] == '\n'){
            i++;
            index++;
            if(i == counter)
                break;
        }

        *countries[i] = *countries[i] + allCountries[index];
        index++;
    }
    *size = counter;
    closedir(dir);
    return countries;
}

void initializeNamedPipes(string* wpipes, string* rpipes, int* writefd, int* readfd, int size){
    for(int i = 0; i < size; i++){
        string writeTemp = "./tmp/w_" + to_string(i);
        string readTemp = "./tmp/r_" + to_string(i);
        const char* wName = writeTemp.c_str();
        const char* rName = readTemp.c_str();

        mkfifo(wName, 0777);
        mkfifo(rName, 0777);

        wpipes[i] = wName;
        rpipes[i] = rName;

        //writefd[i] = open(wName, O_WRONLY | O_NONBLOCK);
        readfd[i] = open(rName, O_RDONLY | O_NONBLOCK);
    }
}

//reading bufferSize every time, message ends when last character is '\n', after that the message is built
int readLine(string* message, int fd, char* buffer, int bufferSize){  
    string temp = "";
    //cout << "ReadLine" << endl;
    int currentBytesRead = 0;
    int totalBytesRead = 0;
    while(currentBytesRead = read(fd, buffer+totalBytesRead, bufferSize-totalBytesRead)){
        totalBytesRead += currentBytesRead;
        if(totalBytesRead < bufferSize){
            continue;
        }

        totalBytesRead = 0;
        currentBytesRead = 0;
        //cout << buffer << "<--- THIS IS BUFFER" << endl;
        if(buffer[0] == '^'){
            return 0;
        }
        char buffernew[bufferSize+1];
        memcpy(buffernew, buffer, bufferSize);
        buffernew[bufferSize] = '\0';
        temp.append(buffernew);
        //temp = temp + buffer;
        //cout << temp << "<-------- THIS IS TEMP + BUFFER FROM READ" << endl;
        if(buffer[bufferSize-1] == '\0'){
            break;
        }
    }

    int i = 0;
    while(1){
        if(temp[i] == ' '){
            do{
                i++;
            }while(temp[i] == ' ');
            if(temp[i] == '\0'){
                break;
            }
            *message = *message + " ";
            continue; //may need to change
        }

        if(temp[i] == '\0'){
            break;
        }
        *message = *message + temp[i];
        i++;
    }
    //cout << *message << "<--- THIS IS THE WHOLE MESSAGE BUILT" << endl;
    return 1;
}

int writeLine(string* message, int fd, char* buffer, int bufferSize){
    //cout << "THIS IS THE MESSAGE THAT I WANT TO SEND------------------------->" << *message << endl;
    int length = strlen(message->c_str());
    //cout << length << endl;
    int i;
    int totalBytesWritten;
    int currentBytesWritten;

    for(i = 0; i < length/bufferSize; i++){
        string temp;
        temp = message->substr(i*bufferSize, bufferSize);
        //cout << temp.c_str() <<  "<-----------THIS IS THE SUBSTRING THAT I AM WRITING"<< endl;
        currentBytesWritten = 0;
        totalBytesWritten = 0;
        while(totalBytesWritten < bufferSize){
            currentBytesWritten += write(fd, temp.c_str() + totalBytesWritten, bufferSize-totalBytesWritten);
            totalBytesWritten += currentBytesWritten;
        }

    }
    
    //like if message is 200 chars long and buffersize 256 then this is 55
    int whitespaces = bufferSize - (length%bufferSize) - 1; 
    
    string temp = "";
    temp = message->substr(i*bufferSize, length - i*bufferSize);
    for(int j = 0; j < whitespaces; j++){
        temp = temp + " ";
    }

    temp = temp + "\0";
    //cout << temp.c_str() <<  "<-----------THIS IS THE SUBSTRING THAT I AM WRITING"<< endl;
    currentBytesWritten = 0;
    totalBytesWritten = 0;
    while(totalBytesWritten < bufferSize){
        currentBytesWritten += write(fd, temp.c_str() + totalBytesWritten, bufferSize-totalBytesWritten);
        totalBytesWritten += currentBytesWritten;
    }
    return 0;

}

int initializeNewChild(string** countries, int countriesNum, string* WRITEnamedPipe, string* READnamedPipe,char* buffer, int bufferSize, string input_dir, int* WRITEfd, int* READfd,int* childPid, int i, int numWorkers){
    pid_t pid;
    string writeTemp = "./tmp/w_" + to_string(i);
    string readTemp = "./tmp/r_" + to_string(i);
    const char* wName = writeTemp.c_str();
    const char* rName = readTemp.c_str();

    mkfifo(wName, 0777);
    mkfifo(rName, 0777);

    *WRITEnamedPipe = wName;
    *READnamedPipe = rName;

            //writefd[i] = open(wName, O_WRONLY | O_NONBLOCK);
    *READfd = open(rName, O_RDONLY | O_NONBLOCK);
            /////////////////////////////////////////////////


    if ((pid = fork()) < 0) {
        perror("Could not fork, please create clients manually");
    }

    else if (pid == 0) {
                // We're in the child process, start a peer
        execl("./worker", WRITEnamedPipe->c_str(), READnamedPipe->c_str(), to_string(bufferSize).c_str(), input_dir.c_str(),(const char*)nullptr);
        exit(127);
    }

    *childPid = pid;
    *WRITEfd = open(WRITEnamedPipe->c_str(), O_WRONLY);

    fcntl(*READfd, F_SETFL, O_RDONLY);
    string message = "";
    string end = "^";
    for(int j = i; j < countriesNum; j = j + numWorkers){ //probably righ
        //cout << j << endl;
        //cout << *countries[j] << endl;
        message = *countries[j] + " ";
        writeLine(&message, *WRITEfd, buffer, bufferSize);
    }
    cout << "END COUNTRIES" << endl;
    writeLine(&end, *WRITEfd, buffer, bufferSize);
    cout << "Reading..." << endl;
    while(readLine(&message, *READfd, buffer, bufferSize));
    //cout << "Finished" << endl;
    while(readLine(&message, *READfd, buffer, bufferSize));//??????????
    //while(readLine(&message, *READfd, buffer, bufferSize));


    //cout << "THIS IS THE MESSAGE" << endl;
    //cout << message << endl;

    return 0;
}

void getLogFileInfo(int* childrenPids, int numWorkers,int* total, int* success, int* fail){
    for(int i = 0; i < numWorkers; i++){
        int workerID = childrenPids[i];
        string logfileName = "log_file." + to_string(workerID);
        string line;
        ifstream myfile;
        myfile.open(logfileName.c_str());
        while(getline(myfile,line)){
            if(line.find("TOTAL") != string::npos){
                break;
            }
        }
        
        int num;
        stringstream ss;
        string temp;                
        ss << line;
                
        while(!ss.eof()){
            ss >> temp;
            if(stringstream(temp) >> num){
                *total = *total + num;
            }
        }

        stringstream ss2;
        getline(myfile,line);

        ss2 << line;
                
        while(!ss2.eof()){
            ss2 >> temp;
            if(stringstream(temp) >> num){
                *success = *success + num;
            }
        }

        getline(myfile,line);
        stringstream ss3;

        ss3 << line;
                
        while(!ss3.eof()){
            ss3 >> temp;
            if(stringstream(temp) >> num){
                *fail = *fail + num;
            }
        }
        
        

        myfile.close();

    }
}

int establishConnection(int portNum, string IP){
    //from here
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(portNum); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, IP.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
    //until here it's the initialization of the socket connection
    cout << IP.c_str() << " - " << portNum << endl;
    return sock;
}

int createConnection(int portNum){
    int server_fd, new_socket, valread; 
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
    address.sin_port = htons( portNum ); 
    
    if (bind(server_fd, (struct sockaddr *)&address,  
                                sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                    (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    cout << "Found connection... " << endl;
    return new_socket; 
}

int getIp_Port(string* ip_port, string* ip, int* port){
    int flag = 0;
    *ip = ip_port->substr(0,ip_port->find("/"));
    *port = atoi(ip_port->substr(ip_port->find("/")+1, ip_port->length()-1).c_str());

    return 1;
}

