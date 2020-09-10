#include "auxilliaryWorker.h"
#include <dirent.h>
#include <fstream>
#include <string>
#include <string.h>
#include <bits/stdc++.h>
#include "Auxilliary.h"
#include "auxilliaryShared.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int ageRanges[] = {0,20,21,40,41,60};
int ageRangesSize = 6;


Record* InitializeRec2(string line,LinkedList<Record*> *recs, string* date, string* countryid){
        // word variable to store word 
    string word, recID, state,patientFirstName, patientLastName, diseaseID,country, entryDate, exitDate; 
    int age;


  
    // making a string stream 
    stringstream iss(line); 
  
    // getting data from the line
    iss >> word;
    recID = word;
    iss >> word;
    state = word;
    iss >> word;
    patientFirstName = word;
    iss >> word;
    patientLastName = word;
    iss >> word;
    diseaseID = word;
    iss >> word;
    age = atoi(word.c_str());
    iss >> word;

    country = *countryid;

    //checking if it should be entered, updated or altogether trashed
    if(state == "ENTER"){
        LinkedListNode<Record*>* temp = recs->GetFirstNode();
        while(temp != NULL && *temp->GetItem()->GetRecId() != recID){
            //cout << temp->GetItem()->GetPatientFirstName() << endl;
            temp = temp->GetNextNode();
        }
        if(temp != NULL /*&& *temp->GetItem()->GetExitDate() != "-"*/){ //??????
            //cout << *temp->GetItem()->GetExitDate() << endl;
            //cout << "Illegal insertion" << endl;
            return NULL;
        }
        //cout << "Inserting" << endl;
        entryDate = *date;
        exitDate = "--";
    }else{
        LinkedListNode<Record*>* temp = recs->GetFirstNode();
        while(temp != NULL && *temp->GetItem()->GetRecId() != recID){
            //cout << "Next for exit" << endl;
            temp = temp->GetNextNode();
        }
        if(temp == NULL){
            //cout << "EXIT BEFORE ENTRY / ILLEGAL DYING TECHNIQUES" << endl;
            return NULL;
        }else{
            temp->GetItem()->SetExitDate(date);
            //cout << "Updated exit Date" << endl;
            return NULL;
        }
    }


    Record* newRec = new Record(recID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate,age);
    //cout << *newRec->GetRecId() << " - " << *newRec->GetCountry() << " - " << *newRec->GetDiseaseID() << endl;
    return newRec;
}

//A function to determine if a date is after another
bool DateComparison2(string* Date1,string* Date2){
    if (*Date2 == "--"){
        //cout << "Patient is still in the hospital!" << endl;
        return false;
    }else if(*Date1 == "--"){
        return true;
    }
    string strNum1 = Date1->substr(6,4)+ Date1->substr(3,2) + Date1->substr(0,2);
    string strNum2 = Date2->substr(6,4)+ Date2->substr(3,2) + Date2->substr(0,2);
    int number1 = stoi(strNum1);
    int number2 = stoi(strNum2);
    //cout << strNum1 << "  " << strNum2 << endl;
    return number1 < number2;
}

//this one is pretty much the same, just modified for qsort
int DateComparison3(const void* Date1,const void* Date2){
    if (**((string**)Date2) == "--"){
        //cout << "Patient is still in the hospital!" << endl;
        return -1;
    }else if(**((string**)Date1) == "--"){
        return 1;
    }
    string strNum1 = (*(string**)Date1)->substr(6,4)+ (*(string**)Date1)->substr(3,2) + (*(string**)Date1)->substr(0,2);
    string strNum2 = (*(string**)Date2)->substr(6,4)+ (*(string**)Date2)->substr(3,2) + (*(string**)Date2)->substr(0,2);
    int number1 = stoi(strNum1);
    int number2 = stoi(strNum2);
   //cout << strNum1 << "  " << strNum2 << endl;
    return number1 - number2;
}


//Getting the concatenated countries from a big string and creating a nice array
string** initializeCountries(string* countries, int* length){
    
    int countriesNum = 0;
    const char* countriesChars = countries->c_str();
    for(int i = 0; i < strlen(countriesChars); i++){
        if(isupper(countriesChars[i])){
            countriesNum++;
        }
    }

    int j = 0;
    string** countriesArray = (string**)malloc(sizeof(string*)*countriesNum);
    for(int i = 0; i < countriesNum; i++){
        countriesArray[i] = new string();
        *countriesArray[i] = "";
        
        do{
            *countriesArray[i] = *countriesArray[i] + countriesChars[j]; 
            j++;
        }while((j < strlen(countriesChars) && (!isupper(countriesChars[j]))));
    }
    *length = countriesNum;
    return countriesArray;
}


//getting the dates from inside the directory-country
string** getDates(string* country, char* input_dir,int* size){
    DIR *dir;
    struct dirent *dent;
    string str(input_dir);
    string dirString = "./" + str + "/" + *country;
    const char* directory = dirString.c_str();
    dir = opendir(directory);
    int counter = 0;
    string allDates = "";

    if(dir!=NULL){
        while((dent=readdir(dir))!=NULL){
            if(dent->d_name[0] != '.'){
                counter++;
                allDates = allDates + dent->d_name + "\n";
            }     
        }
    }

    string** dates = (string**)malloc(sizeof(string*)*counter);
    for(int i = 0; i < counter; i++){
        dates[i] = new string();
    }
    for(int i = 0; i < counter; i++){
        *dates[i] = "";
    }
    int i = 0;
    int index = 0;
    while(i < counter){
        if(allDates[index] == '\n'){
            i++;
            index++;
            if(i == counter)
                break;
        }

        *dates[i] = *dates[i] + allDates[index];
        index++;
    }
    *size = counter;
    closedir(dir);
    return dates;
}


//Getting the dates from the countries-subdirectories, initializing the records and then organize them date sorted for each country
LinkedList<Record*>* openFilesFromCountryDirsSorted(string** countries, int countriesLength, string** latestDates, char* input_dir,HashTable** diseaseHashTable, HashTable** countryHashTable, int WRITEfd, char* buffer, int bufferSize){
    LinkedList<Record *> *test = new LinkedList<Record *>();
    LinkedListNode<Record* > *current;


    for(int i = 0; i < countriesLength; i++){

        //opening specific country's files in dating order!!!!
        int size;
        string** dates = getDates(countries[i], input_dir,&size);

        //sort(dates, dates+size,DateComparison2);
        qsort(dates,size,sizeof(string*), DateComparison3);

        *latestDates[i] = *dates[size-1];
        //cout << "LATEST DATE -> " << *latestDates[i] << endl;


        // cout << *dates[0] <<endl;
        // cout << size << endl;
        // for(int j = 0; j < size; j++){
        //     // if(DateComparison(dates[j], dates[j-1])){
        //     //     //cout << "OPA BRO" << endl;
        //     // }
        //     cout << j << " " << *dates[j] << endl;
        // }

        //LinkedList of recs for country[i]

        for(int j = 0; j < size; j++){
            string str(input_dir);
            string dirString = "./" + str + "/" + *countries[i]+ "/" + *dates[j];
            string line;

            string* statistics = new string();
            *statistics = "";

            ifstream myfile;
            myfile.open(dirString.c_str());

            if (!myfile.is_open())
            {
                cout << "ERROR OPENING FILE -- ABORTING PROGRAM!" << endl;
                exit(EXIT_FAILURE);
            }

            while (getline(myfile, line))
            {
                Record *newRec = InitializeRec2(line,test, dates[j], countries[i]);
                if (newRec != NULL)
                {
                    test->Insert(newRec);
                    createStatistics(newRec, statistics);
                }
                else
                {
                    //pretty much not needed really
                   // cout << "Error" << endl;
                }
            }
            
            myfile.close();
            //cout << *statistics << endl;
            //PrintHashTable(*diseaseHashTable,256);
            //string* messageForParent = new string();
             if(*statistics != ""){
                organizeAndSendStats(statistics, countries[i], dates[j], ageRanges, ageRangesSize, WRITEfd, buffer, bufferSize);
             }
            delete statistics;
        }
        for(int j = 0; j < size; j++){
            delete(dates[j]);
        }
        free(dates);
        //free(dates);
    }
    LinkedListNode<Record*> * temp = test->GetFirstNode();
    while(temp != NULL){
        Record* newRec = temp->GetItem();
        Record *newRec2 = new Record();
        Record *newRec3 = new Record();

        *newRec2 = *newRec;
        *newRec3 = *newRec;
                        //cout << "THIS IS THE EXIT DATE -> "<< *newRec->GetExitDate() << endl;
        (*diseaseHashTable)->Insert(newRec2, "disease");
        (*countryHashTable)->Insert(newRec3, "country"); 
        temp = temp->GetNextNode();
    }
    string end = "^";
    writeLine(&end, WRITEfd, buffer, bufferSize);

    return test;
}

int createStatistics(Record* rec, string* stats){
    *stats = *stats + *rec->GetDiseaseID() + "/" + to_string(rec->GetAge()) + "^\n";
    return 0;
}

int organizeAndSendStats(string* stats, string* country, string* date, int* ageRanges, int ageRangesSize, int WRITEfd, char* buffer, int bufferSize){
    int counter = 0;

    string diseases = "";
    string message = *date + "\n" + *country + "\n";

    istringstream statsStream(*stats);
    string line;

    while(getline(statsStream,line)){
        string disease = "";
        for(int i = 0; i < strlen(line.c_str()); i++){
            if(line[i] == '/'){
                break;
            }
            disease = disease + line[i];
        }
        size_t found = diseases.find(disease); // ???
        if(found == string::npos){
            counter++;
            diseases = diseases + disease + "\n";
        }
    }

    string diseasesArray[counter];
    istringstream diseasesStream(diseases);
    for(int i = 0; i < counter; i++){
        getline(diseasesStream,line);
        diseasesArray[i] = line;
    }

    int** statsPerDisease = (int**)malloc(sizeof(int*)*counter);
    for(int i = 0; i < counter; i++){
        statsPerDisease[i] = (int*)malloc(sizeof(int)*(1+ageRangesSize/2));
        for(int j = 0; j < 1+ageRangesSize/2; j++){
            statsPerDisease[i][j] = 0;
        }
    }
    


    istringstream finalStatsStream(*stats);

    while(getline(finalStatsStream,line)){
        string age = "";
        string disease = "";
        for(int i = 0; i < strlen(line.c_str()); i++){
            if(line[i] == '/'){
                int j = i+1;
                do{
                    age = age + line[j];
                    j++;
                }while(line[j] != '^');

                break;
            }
            disease = disease + line[i];
        }
        
        for(int i = 0; i < counter; i++){
            if(diseasesArray[i] == disease){
                int j;
                for(j = 0; j < ageRangesSize; j = j+2){
                    if(ageRanges[j] <= atoi(age.c_str()) && ageRanges[j+1] >= atoi(age.c_str())){
                        statsPerDisease[i][j/2] = statsPerDisease[i][j/2]+1;
                        break;
                    }
                }
                if(ageRangesSize == j){
                    statsPerDisease[i][ageRangesSize/2] = (statsPerDisease[i][ageRangesSize/2]) + 1;
                }
            }
        }
    }

    for(int i = 0; i < counter; i++){
        message = message + diseasesArray[i] + "\n";
        for(int j = 0; j < ageRangesSize; j = j+2){
            message = message + "Age range " + to_string(ageRanges[j]) + "-" + to_string(ageRanges[j+1]) + " years: " + to_string(statsPerDisease[i][j/2]) + " cases\n";
        }
        message = message + "Age range " + to_string(ageRanges[ageRangesSize-1]) + "+ years: " + to_string(statsPerDisease[i][ageRangesSize/2]) + " cases\n\n";
    }

    for(int i = 0; i < counter; i++){
        free(statsPerDisease[i]);
    }
    free(statsPerDisease);

    writeLine(&message, WRITEfd, buffer, bufferSize);

    return 0;    
}

void topkCounter(int* ageCategories, AVLnode* node, string* disease, int* allPatients,string* date1, string* date2){
    if(node != NULL){
        topkCounter(ageCategories, node->GetLeftChild(), disease, allPatients, date1, date2);
        LinkedListNode<Record*>* records = node->GetRecords()->GetFirstNode();
        //cout << "Printing List" << endl;
        do{
            //cout << *node->GetRecords()->GetFirstNode()->GetItem()->GetCountry() << " - " << *node->GetRecords()->GetFirstNode()->GetItem()->GetDiseaseID() << " - " << node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate() << " - " <<  *node->GetRecords()->GetFirstNode()->GetItem()->GetExitDate()<< endl;
            if(*(records->GetItem()->GetDiseaseID()) == *disease){
                int age = records->GetItem()->GetAge();

                //cout << records->GetItem()->GetEntryDate() << endl;

                if(age > ageRanges[ageRangesSize-1] && DateComparison(records->GetItem()->GetEntryDate(),*date1) && DateComparison(*date2,records->GetItem()->GetEntryDate())){
                    ageCategories[ageRangesSize/2]++;
                    *allPatients = *allPatients + 1;
                }else{
                    for(int i = 0; i < ageRangesSize; i = i+2){
                        if(age >= ageRanges[i] && age <= ageRanges[i+1] && DateComparison(records->GetItem()->GetEntryDate(),*date1) && DateComparison(*date2,records->GetItem()->GetEntryDate())){
                            ageCategories[i/2]++;
                            *allPatients = *allPatients + 1;
                            break;
                        }
                    }
                }
                
            }
            records = records->GetNextNode();
        }while(records != NULL);
        
        topkCounter(ageCategories,node->GetRightChild(), disease, allPatients,date1, date2);
    }
}

//this is a comparison function for qsort
int cmpfunc (const void * a, const void * b) {
   return -( *(int*)a - *(int*)b );
}

string topkAgeRanges(int k, string* country, string* disease, string* date1, string* date2, HashTable* countryHashTable){
    if(k > ageRangesSize/2 + 1){
        cout << "ERROR ~ k is bigger than the set age ranges...\n" << endl;
        return "";
    }

    string messageForParent = "";

    int bucketNum = countryHashTable->Hash(*country);
    //cout << "THIS IS THE WANTED BUCKET ----> " << bucketNum << endl;
    Bucket* bucket = countryHashTable->GetListOfBuckets()[bucketNum];
    //cout << "GOT THE BUCKET" << endl;
    int i;
    for(i = 0; i < bucket->GetNumEntries(); i++){
        if(*bucket->GetBucketEntries()[i]->GetData() == *country){
            break;
        }else if(i == bucket->GetNumEntries()-1){
            bucket = bucket->GetNextBucket();
            i = -1;
            if(bucket == NULL){
                cout << "NOT FOUND ~ ERROR" << endl;
                return "";
            }
        }
    }
    AVLnode* node = bucket->GetBucketEntries()[i]->GetTree()->GetRoot();

    int ageCategories[ageRangesSize/2 + 1];
    int allPatients = 0;

    for(i = 0; i < ageRangesSize/2 + 1; i++){
        ageCategories[i] = 0;
    }
    //  for(i = 0; i < ageRangesSize/2 + 1; i++){
    //     cout << ageCategories[i] << endl;
    // }



    topkCounter(ageCategories, node, disease, &allPatients, date1, date2);

    if(allPatients == 0){
        //cout << "No patients" << endl;
        return "";
    }
    // cout << "ALL PATIENTS ARE: " << allPatients << endl;
    // for(i = 0; i < ageRangesSize/2 + 1; i++){
    //     cout << ageCategories[i] << endl;
    // }
    
    //copying the values before sorting
    int ageCategoriesCOPY[ageRangesSize/2 + 1];
    for(int i = 0; i < ageRangesSize/2 + 1; i++){
        ageCategoriesCOPY[i] = ageCategories[i];
    }


    //sort(ageCategories, ageCategories + ageRangesSize/2 + 1, greater<int>());
    qsort(ageCategories,ageRangesSize/2 + 1, sizeof(int), cmpfunc);
    
    int flag[ageRangesSize/2 +1];
    for(int j = 0; j < ageRangesSize/2+1; j++){
        flag[j] = 0;
    }
    int j;
    int counter = 0;
    for(j = 0; j < ageRangesSize/2+1; j++){
        if(counter == k){
            break;
        }
        if(ageCategoriesCOPY[ageRangesSize/2] == ageCategories[j] && flag[ageRangesSize/2] == 0){
            //cout << ageRanges[ageRangesSize-1] << "+: " <<  ((float)ageCategories[j]/(float)allPatients)*100 << "%\n" << endl;
            messageForParent = messageForParent + to_string(ageRanges[ageRangesSize-1]) + "+: " + to_string(((float)ageCategories[j]/(float)allPatients)*100) + "%\n";
            flag[ageRangesSize/2] = 1;
            counter++;
        }
        for(i = 0; i < ageRangesSize; i = i+2){
            if(ageCategories[j] == ageCategoriesCOPY[i/2] && flag[i/2] == 0){
                //cout << ageRanges[i] << "-" <<ageRanges[i+1] << ": " <<  ((float)ageCategories[j]/(float)allPatients)*100 << "%\n" << endl;
                messageForParent = messageForParent + to_string(ageRanges[i]) + "-" + to_string(ageRanges[i+1]) + ": " + to_string(((float)ageCategories[j]/(float)allPatients)*100) + "%\n";

                flag[i/2] = 1;
                counter++;
                if(counter == k){
                    break;
                }
            }
        }
    }
    //cout << messageForParent << endl;
    return messageForParent;
}



void CountPatientsAdmissions(AVLnode* node,int* counter, string* date1, string* date2, string* country){
    
    if(node != NULL){

        if(!DateComparison(*date1, node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate()) && !DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), *date2)){
            if (*node->GetRecords()->GetFirstNode()->GetItem()->GetCountry() == *country){
                LinkedListNode<Record*>* records = node->GetRecords()->GetFirstNode();
                do{
                    *counter = *counter+1;
                    records = records->GetNextNode();
                }while(records != NULL);
            }
        }
        CountPatientsAdmissions(node->GetLeftChild(), counter, date1, date2, country);
        CountPatientsAdmissions(node->GetRightChild(), counter, date1, date2, country);

    }
    
}

string numPatientsAdmissions(string* disease, string* date1, string* date2, string* country, HashTable* diseaseHashTable){
    string message = "";
    int bucketNum = diseaseHashTable->Hash(*disease);
    Bucket* bucket = diseaseHashTable->GetListOfBuckets()[bucketNum];
    int i=0;
    for(i = 0; i < bucket->GetNumEntries(); i++){
        if(*bucket->GetBucketEntries()[i]->GetData() == *disease){
            break;
        }else if(i == bucket->GetNumEntries()-1){
            bucket = bucket->GetNextBucket();
            i = -1;
            if(bucket == NULL){
                cout << "NOT FOUND ~ ERROR" << endl;
                return message;
            }
        }
    }

    AVLnode* node = bucket->GetBucketEntries()[i]->GetTree()->GetRoot();

    int counter = 0;

    CountPatientsAdmissions(node, &counter, date1, date2, country);
    message = *country + " " + to_string(counter) + "\n";
    return message;
}

void CountPatientsDischarges(AVLnode* node,int* counter, string* date1, string* date2, string* country){
    
    if(node != NULL){
        if (*node->GetRecords()->GetFirstNode()->GetItem()->GetCountry() == *country){
            LinkedListNode<Record*>* records = node->GetRecords()->GetFirstNode();
            do{
                //cout << *records->GetItem()->GetRecId() << " - "<< *records->GetItem()->GetExitDate() << endl;
                if(*(records->GetItem()->GetExitDate()) != "--"){
                    if(!DateComparison(*date1, *(records->GetItem()->GetExitDate())) && !DateComparison(*(records->GetItem()->GetExitDate()), *date2)){
                        *counter = *counter+1;
                    }
                }
                records = records->GetNextNode();
            }while(records != NULL);
        }
        
        CountPatientsDischarges(node->GetLeftChild(), counter, date1, date2, country);
        CountPatientsDischarges(node->GetRightChild(), counter, date1, date2, country);

    }
    
}

string numPatientsDischarges(string* disease, string* date1, string* date2, string* country, HashTable* diseaseHashTable){
    string message = "";
    int bucketNum = diseaseHashTable->Hash(*disease);
    Bucket* bucket = diseaseHashTable->GetListOfBuckets()[bucketNum];
    int i=0;
    for(i = 0; i < bucket->GetNumEntries(); i++){
        if(*bucket->GetBucketEntries()[i]->GetData() == *disease){
            break;
        }else if(i == bucket->GetNumEntries()-1){
            bucket = bucket->GetNextBucket();
            i = -1;
            if(bucket == NULL){
                cout << "NOT FOUND ~ ERROR" << endl;
                return message;
            }
        }
    }

    AVLnode* node = bucket->GetBucketEntries()[i]->GetTree()->GetRoot();

    int counter = 0;

    CountPatientsDischarges(node, &counter, date1, date2, country);
    message = *country + " " + to_string(counter) + "\n";
    return message;
}


void updateRecords(string** country, int numCountries, string** latestDatesPerCountry,char* inputDirr, LinkedList<Record*>* recList, HashTable** diseaseHashTable, HashTable** countryHashTable, int WRITEfd, char* buffer, int bufferSize){
    for(int i = 0; i < numCountries; i++){
        int size;
        string** dates = getDates(country[i], inputDirr, &size);
        //sort(dates, dates+size,DateComparison2);
        qsort(dates,size,sizeof(string*), DateComparison3);

        if(*latestDatesPerCountry[i] == *dates[size-1]){
            //cout << "OLA GUCCI FAM" << endl;
            for(int j = 0; j < size; j++){
                delete(dates[j]);
            }
            free(dates);
            continue;
        }
        string newLatestDate = *dates[size-1];
        string str(inputDirr);
        string dirString = "./" + str + "/" + *country[i]+ "/" + *dates[size-1];
        string line;

        string* statistics = new string();
        *statistics = "";

        ifstream myfile;
        myfile.open(dirString.c_str());

        if (!myfile.is_open())
        {
            cout << "ERROR OPENING FILE -- ABORTING PROGRAM!" << endl;
            exit(EXIT_FAILURE);
        }
        while (getline(myfile, line))
        {
            Record *newRec = InitializeRec2(line,recList, dates[size-1], country[i]);
            if (newRec != NULL)
            {
                recList->Insert(newRec);
                createStatistics(newRec, statistics);
            }
            else
            {
                    //pretty much not needed really
                   // cout << "Error" << endl;
            }
        }
        myfile.close();
        //string* messageForParent = new string();
        if(*statistics != ""){
            organizeAndSendStats(statistics, country[i], dates[size-1], ageRanges, ageRangesSize, WRITEfd, buffer, bufferSize);
        }
        delete statistics;
        for(int j = 0; j < size; j++){
            delete(dates[j]);
        }
        free(dates);

         LinkedListNode<Record*> * temp = recList->GetFirstNode();
        while(temp != NULL){
            Record* newRec = temp->GetItem();
            if(DateComparison(newRec->GetEntryDate(), *latestDatesPerCountry[i])){
                Record *newRec2 = new Record();
                Record *newRec3 = new Record();

                *newRec2 = *newRec;
                *newRec3 = *newRec;
                                //cout << "THIS IS THE EXIT DATE -> "<< *newRec->GetExitDate() << endl;
                (*diseaseHashTable)->Insert(newRec2, "disease");
                (*countryHashTable)->Insert(newRec3, "country");
            } 
            temp = temp->GetNextNode();
        }
        *latestDatesPerCountry[i] = newLatestDate;
    }

    
    string end = "^";
    writeLine(&end, WRITEfd, buffer, bufferSize);
    return;
}