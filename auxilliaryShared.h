#include <iostream>
#include <string.h>
#include <string>

using namespace std;

#define SIZE 128


string** getCountries(string* directory,int* size);
void initializeNamedPipes(string* wpipes, string* rpipes, int* writefd, int* readfd,int size);

int readLine(string* message, int fd, char* buffer, int bufferSize);
int writeLine(string* message, int fd, char* buffer, int bufferSize);

int initializeNewChild(string** countries, int countriesNum, string* WRITEnamedPipe, string* READnamedPipe,char* buffer, int bufferSize, string input_dir, int* WRITEfd, int* READfd,int* childPid, int i, int numWorkers);
void getLogFileInfo(int* childrenPids, int numWorkers,int* total, int* success, int* fail);
int establishConnection(int portNum, string IP);
int createConnection(int portNum);

int getIp_Port(string*, string*, int*);