#include "clientAuxilliary.h"
#include <fstream>
#include <iostream>

using namespace std;

int processQueries(string file, string*** queries,int* numOfQueries){
    fstream myfile;
    myfile.open(file);
    if(!myfile.is_open()){
        return 1;
    }
    string line;
    int num = 0;

    while(getline(myfile, line)){
        num++;
    }
    myfile.close();

    //initializing the query array
    *queries = (string**)malloc(num*sizeof(string*));
    cout << "THIS IS THE NUM -> " << num << endl;
    for(int i = 0; i < num; i++){
        (*queries)[i] = new string();
    }
    //reopening to get the queries
    fstream myfileREAD;
    myfileREAD.open(file);
    if(!myfileREAD.is_open()){
        return 1;
    }

    int i = 0;
    while(getline(myfileREAD,line)){
        *(*queries)[i] = line;
        i++;

    }
    myfileREAD.close();
    *numOfQueries = num;

    return 0;
}
