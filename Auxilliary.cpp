#include <iostream>
#include "Auxilliary.h"
#include "HashTable.h"


using namespace std;

bool DateComparison(string Date1,string Date2){
    if (Date2 == "-"){
        //cout << "Patient is still in the hospital!" << endl;
        return false;
    }else if(Date1 == "-"){
        return true;
    }
    string strNum1 = Date1.substr(6,4)+ Date1.substr(3,2) + Date1.substr(0,2);
    string strNum2 = Date2.substr(6,4)+ Date2.substr(3,2) + Date2.substr(0,2);
    int number1 = stoi(strNum1);
    int number2 = stoi(strNum2);
   //cout << strNum1 << "  " << strNum2 << endl;
    return number1 > number2;
}

bool RecIdComparison(Record* rec1, Record* rec2){
   string* id1 = rec1->GetRecId();
   string* id2 = rec2->GetRecId();

   return (*id1 == *id2);
}

Record* InitializeRec(string line){
        // word variable to store word 
    string word, recID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate; 


  
    // making a string stream 
    stringstream iss(line); 
  
    // getting data from the line
    iss >> word;
    recID = word;
    iss >> word;
    patientFirstName = word;
    iss >> word;
    patientLastName = word;
    iss >> word;
    diseaseID = word;
    iss >> word;
    country = word;
    iss >> word;
    entryDate = word;
    iss >> word;
    exitDate = word;
    
    Record* newRec = new Record(recID, patientFirstName, patientLastName, diseaseID, country, entryDate, exitDate, 0);
    
    return newRec;
}


int max(int a, int b){
    if(a > b){
        return a;
    }else{
        return b;
    }
}

void printAVLtree(AVLnode* node){
    
    if(node != NULL){
            //cout << "AIGHT FAM" << endl;

        printAVLtree(node->GetLeftChild());
        LinkedListNode<Record*>* records = node->GetRecords()->GetFirstNode();
        cout << "Printing List" << endl;
        do{
            cout << *node->GetRecords()->GetFirstNode()->GetItem()->GetCountry() << " - " << *node->GetRecords()->GetFirstNode()->GetItem()->GetDiseaseID() << " - " << node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate() << " - " <<  *node->GetRecords()->GetFirstNode()->GetItem()->GetExitDate()<< endl;
            records = records->GetNextNode();
        }while(records != NULL);
        
        printAVLtree(node->GetRightChild());

    }
    
}

    // testin bucket
void BucketTest(int size){

    Bucket* testBucket = new Bucket();
    string muni[size];

    for(int i = 0; i < size ; i++){
        muni[i] = to_string(i);
        cout << muni[i] << endl;
        BucketEntry* testEntry = new BucketEntry(&muni[i]);
        testBucket->Insert(testEntry);
    }    
    int counter = 0;
    cout << "print BUCKET number 0 with adress: "<< testBucket << endl;
    BucketEntry** br =  testBucket->GetBucketEntries();
    for (int i = 0; i < testBucket->GetNumEntries(); i++){

        cout << *br[i]->GetData() << endl;

        if(i == testBucket->GetNumEntries()-1 && testBucket->GetNextBucket() != NULL){
            i = -1;
            testBucket = testBucket->GetNextBucket();
            counter++;
            br =  testBucket->GetBucketEntries();
            cout << "print BUCKET number: " << counter << " with adress:  " << testBucket<<  endl;
        
        }
    }
    cout << "Bucket Capacity: "  << testBucket->GetCapacity() << endl;
    cout << "Num entries: " << testBucket->GetNumEntries() << endl;
}

void PrintHashTable(HashTable* table, int num){
    Bucket** buckets = table->GetListOfBuckets(); // get the list of buckets from the hash table
    for(int i = 0; i < num; i++){ //iterate through the list of bucket lists
        cout << "Number of position in ListOfBuckets: " << i << endl;
        int counter = 0;
        Bucket* currBucket = NULL;
        if(buckets[i] != NULL)
            currBucket = buckets[i];
        if(currBucket == NULL){
            cout << "Not initialized -- continuing" << endl;
            continue;
        }
        BucketEntry** currEntries = currBucket->GetBucketEntries();
        cout << "Printing bucket with number: 0" << endl; 
        for(int j = 0; j < currBucket->GetNumEntries(); j++){
            cout << endl << "COUNTRY / DISEASE : ";
            cout << *currEntries[j]->GetData() << endl;
            cout << endl << endl ;
            cout << "\t PRINTING AVL TREE" << endl << endl;
            printAVLtree(currEntries[j]->GetTree()->GetRoot());
            if(j == currBucket->GetNumEntries()-1 && currBucket->GetNextBucket() != NULL){
                cout << "BRUH" << endl;
                j = -1;
                currBucket = currBucket->GetNextBucket();
                counter++;
                currEntries =  currBucket->GetBucketEntries();
                cout << "print BUCKET number: " << counter << " with adress:  " << currBucket<<  endl;
        
             }  
        }
    }
}

void PrintHeap(HeapNode* node, int space){

    if(node == NULL){
        return;
    }

    space += 10;
    
    PrintHeap(node->GetRightChild(),space);

    cout << endl;
    for(int i = 10; i < space; i++){
        cout << " ";
    }
    cout << *node->GetID() << " / " << node->GetCounter() << endl;

    PrintHeap(node->GetLeftChild(),space);
}

void Count(AVLnode* node,int* counter, string* date1, string* date2){
    
    
    if(node != NULL){
        
        if(!DateComparison(*date1, node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate()) && !DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), *date2)){
            LinkedListNode<Record*>* records = node->GetRecords()->GetFirstNode();
            do{
                *counter = *counter+1;
                records = records->GetNextNode();
            }while(records != NULL);
        }
        Count(node->GetLeftChild(), counter, date1, date2);
        Count(node->GetRightChild(), counter, date1, date2);

    }
    
}

void Count(AVLnode* node,int* counter, string* date1, string* date2, string* country){
    
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
        Count(node->GetLeftChild(), counter, date1, date2, country);
        Count(node->GetRightChild(), counter, date1, date2, country);

    }
    
}

void GlobalDiseaseStats(HashTable* diseaseHashTable, int tableNumOfEntries, string* date1, string* date2){
    if(date1 != NULL && date2 == NULL){
        cout << "WRONG ARGUMENTS!!! - can't have only entry date ~ returning" << endl;
    }else if(date1 == NULL && date2 == NULL){
        for(int i = 0; i < tableNumOfEntries; i++){
            Bucket* currBucket = diseaseHashTable->GetListOfBuckets()[i];
            if(currBucket != NULL){
                for(int j = 0; j < currBucket->GetNumEntries(); j++){
                    cout << *currBucket->GetBucketEntries()[j]->GetData() << " " << currBucket->GetBucketEntries()[j]->GetTree()->GetSize() << endl;
                }
            }
        }
    }else if(date1 != NULL && date2 != NULL){
        for(int i = 0; i < tableNumOfEntries; i++){
            Bucket* currBucket = diseaseHashTable->GetListOfBuckets()[i];
            if(currBucket != NULL){
                for(int j = 0; j < currBucket->GetNumEntries(); j++){
                    int counter = 0;
                    AVLnode* node = currBucket->GetBucketEntries()[j]->GetTree()->GetRoot();
                    Count(node, &counter, date1, date2);
                    cout << *currBucket->GetBucketEntries()[j]->GetData() << " " << counter << endl;
                }
            }
        }
    }
}

int DiseaseFrequency(HashTable* diseaseHashTable, string virusName, string* country, string date1, string date2){

    int bucketNum = diseaseHashTable->Hash(virusName);
    Bucket* bucket = diseaseHashTable->GetListOfBuckets()[bucketNum];
    int i=0;
    for(i = 0; i < bucket->GetNumEntries(); i++){
        if(*bucket->GetBucketEntries()[i]->GetData() == virusName){
            break;
        }else if(i == bucket->GetNumEntries()-1){
            bucket = bucket->GetNextBucket();
            i = -1;
            if(bucket == NULL){
                cout << "NOT FOUND ~ ERROR" << endl;
                return 0;
            }
        }
    }

    AVLnode* node = bucket->GetBucketEntries()[i]->GetTree()->GetRoot();
    //printAVLtree(node);//to be commented
    //cout << endl << endl;

    int counter = 0;
    if(country == NULL){    
     Count(node, &counter, &date1, &date2);
     //cout << virusName << " " << date1 << " - " << date2 << " is:" << endl;
     //cout << *bucket->GetBucketEntries()[i]->GetData() << " " << counter << endl;          
    }else{
     Count(node, &counter, &date1, &date2, country);
     //cout << "For disease " << virusName << " the number of patients for " << *country << " between the dates " << date1 << " - " << date2 << " is:" << endl;
     //cout << *bucket->GetBucketEntries()[i]->GetData() << " " << counter << endl; 
    }
    return counter;
}

void TopkDiseases(LinkedList<Record*>* list, int k, string country, string* date1, string* date2){
    Heap* heap = new Heap();
    if(date1 != NULL && date2 == NULL){
        cout << "Date 2 not initialized - ERROR returning" << endl;
        return;
    }else if(date1 == NULL && date2 == NULL){
        LinkedListNode<Record*>* node =  list->GetFirstNode();
        while(node != NULL){
            Record* rec = node->GetItem();
            if(country == *rec->GetCountry())
                heap->Update(heap->GetRoot(), rec->GetDiseaseID());
            node = node->GetNextNode();
        }

        //cout << "For " << country << " the top " << k << " diseases are:" << endl;
        for(int i = 0; i < k; i++){
           // PrintHeap(heap->GetRoot(), 10);
            int instances = heap->GetRoot()->GetCounter();
            string* tobeprinted = heap->ExtractMax();
            if(tobeprinted == NULL){
                return;
            }
            cout << *tobeprinted << " " << instances << endl;
        }
    }else if(date1 != NULL && date2 != NULL){
        LinkedListNode<Record*>* node =  list->GetFirstNode();
        while(node != NULL){
            Record* rec = node->GetItem();
            if(country == *rec->GetCountry() && !DateComparison(*date1, rec->GetEntryDate()) && !DateComparison(rec->GetEntryDate(), *date2))
                heap->Update(heap->GetRoot(), rec->GetDiseaseID());
            node = node->GetNextNode();
        }
        
        //cout << "For " << country << " the top " << k << " diseases between the dates " << *date1 << " - " << *date2 << " are:" << endl;
        for(int i = 0; i < k; i++){
           // PrintHeap(heap->GetRoot(), 10);
            int instances = heap->GetRoot()->GetCounter();
            string* tobeprinted = heap->ExtractMax();
            if(tobeprinted == NULL){
                return;
            }
            cout << *tobeprinted << " " << instances << endl;
        }
    }
    delete heap;
}

void TopkCountries(LinkedList<Record*>* list, int k, string disease, string* date1, string* date2){
    Heap* heap = new Heap();
    if(date1 != NULL && date2 == NULL){
        cout << "Date 2 not initialized - ERROR returning" << endl;
        return;
    }else if(date1 == NULL && date2 == NULL){
        LinkedListNode<Record*>* node =  list->GetFirstNode();
        while(node != NULL){
            Record* rec = node->GetItem();
            if(disease == *rec->GetDiseaseID())
                heap->Update(heap->GetRoot(), rec->GetCountry());
            node = node->GetNextNode();
        }

        //cout << "For " << disease << " the top " << k << " countries are:" << endl;
        for(int i = 0; i < k; i++){
           // PrintHeap(heap->GetRoot(), 10);
            int instances = heap->GetRoot()->GetCounter();
            string* tobeprinted = heap->ExtractMax();
            if(tobeprinted == NULL){
                return;
            }
            cout << *tobeprinted << " " << instances << endl;
        }
    }else if(date1 != NULL && date2 != NULL){
        LinkedListNode<Record*>* node =  list->GetFirstNode();
        while(node != NULL){
            Record* rec = node->GetItem();
            if(disease == *rec->GetDiseaseID() && !DateComparison(*date1, rec->GetEntryDate()) && !DateComparison(rec->GetEntryDate(), *date2))
                heap->Update(heap->GetRoot(), rec->GetCountry());
            node = node->GetNextNode();
        }
        
        //cout << "For " << disease << " the top " << k << " countries between the dates " << *date1 << " - " << *date2 << " are:" << endl;
        for(int i = 0; i < k; i++){
           // PrintHeap(heap->GetRoot(), 10);
            int instances = heap->GetRoot()->GetCounter();
            string* tobeprinted = heap->ExtractMax();
            if(tobeprinted == NULL){
                return;
            }
            cout << *tobeprinted << " " << instances << endl;
        }
    }
    delete(heap);
}

Record* Find(LinkedList<Record*>* list, string* id){
    LinkedListNode<Record*>* node = list->GetFirstNode();
    while(node != NULL){
        if(*node->GetItem()->GetRecId() == *id){
            return node->GetItem();
        }
        node = node->GetNextNode();
    }
    return NULL;
}

int NumCurrentPatients(LinkedList<Record*>* list,string* disease){
    LinkedListNode<Record*>* node = list->GetFirstNode();
    int counter = 0;
    if(disease == NULL){
        while(node != NULL){
            string* exitDate;
            if(*node->GetItem()->GetExitDate() == "-"){
                counter++;
            }
            node = node->GetNextNode();
        }
    }else{
        while(node != NULL){
            string* exitDate;
            if(*node->GetItem()->GetExitDate() == "-" && *node->GetItem()->GetDiseaseID() == *disease){
                counter++;
            }
            node = node->GetNextNode();
        }
    }
    return counter;
}

