#include <iostream>
#include <string>
#include "HashTable.h"
#include "AVLtree.h"

using namespace std;

HashTable::HashTable(int diseaseHashtableNumOfEntries){
    this->diseaseHashtableNumOfEntries = diseaseHashtableNumOfEntries;
    this->listOfBuckets = (Bucket**) malloc(diseaseHashtableNumOfEntries * sizeof(Bucket*) /*- sizeof(int) + sizeof(Bucket**)*/);
    //cout<< diseaseHashtableNumOfEntries * sizeof(Bucket*) - sizeof(int) - sizeof(Bucket**)<< endl;
    for(int i = 0; i < diseaseHashtableNumOfEntries; i++){
        this->listOfBuckets[i] = NULL;
    }
    //   for(int i = 0; i < diseaseHashtableNumOfEntries; i++){
    //     if(this->listOfBuckets[i] == NULL){
    //         cout << i << endl;
    //     }
    // }
}

HashTable::~HashTable(){
    for(int i = 0; i < this->diseaseHashtableNumOfEntries; i++){
        delete this->listOfBuckets[i];
    }
    free (this->listOfBuckets);
}

int HashTable::Hash(string stringToBeHashed){
    unsigned int hashVal = 0;
    for(int x = 0; x < stringToBeHashed.length(); x++){
        hashVal ^=(hashVal << 5) + (hashVal >> 2) + stringToBeHashed[x];
    }
    // cout << "identifier " << stringToHash << "hashed to " << hashVal % this->numOfEntries << endl;
    return hashVal % this->diseaseHashtableNumOfEntries;
}

Bucket** HashTable::GetListOfBuckets(){
    return this->listOfBuckets;
}

void HashTable::Insert(Record* data, string flag){
    string* key;
    if (flag.compare("country") == 0){
         key = data->GetCountry();
    }else if(flag.compare("disease") == 0){
         key = data->GetDiseaseID();
    }else{
        cout << "WRONG ARGUMENTS!!! ACCEPTABLE ONLY 'country' AND 'disease' -- Returning!" << endl;
    }
    int bucketNum = Hash(*key);
    //cout << this->diseaseHashtableNumOfEntries << endl;
    //cout << "HASH KEY OF " << *key << " IS -> " << bucketNum << endl;
    if(this->listOfBuckets[bucketNum] == NULL){
        Bucket* newBucket = new Bucket(); // create a new bucket
        AVLtree* newTree = new AVLtree(); // create a new tree

        newTree->SetRoot( newTree->Insert(newTree->GetRoot(), data)); // insert data into avl tree
        BucketEntry* newEntry = new BucketEntry(key, newTree); // creating a new bucket entry
        newBucket->Insert(newEntry); // inserting entry into the new bucket

        this->listOfBuckets[bucketNum] = newBucket; //
        //cout << "Pos of list is empty -- Creating new bucket succesfully!" << endl;
        return ;
    }else{
        Bucket* tempBucket = this->listOfBuckets[bucketNum];
        for(int i = 0; i < tempBucket->GetNumEntries(); i++){
            BucketEntry* tempEntry = tempBucket->GetBucketEntries()[i];
            if(key->compare(*(tempEntry->GetData())) == 0){
                AVLtree* tempTree = tempEntry->GetTree();
                tempTree->SetRoot( tempTree->Insert(tempTree->GetRoot(), data)); // insert data into avl tree
                //cout << "Found same Data!  ->  " << *key <<" = " << *(tempEntry->GetData())<<endl;
                return;
            }
            if(tempBucket->GetNextBucket() != NULL && i == tempBucket->GetNumEntries()-1){
                tempBucket = tempBucket->GetNextBucket();
                i = -1;
                //cout << "Going to next bucket!" << endl;
            }else if (tempBucket->GetNextBucket() == NULL && i == tempBucket->GetNumEntries()-1){
                AVLtree* newTree = new AVLtree(); // create a new tree
                newTree->SetRoot( newTree->Insert(newTree->GetRoot(), data)); // insert data into avl tree
                BucketEntry* newEntry = new BucketEntry(key, newTree); // creating a new bucket entry
                tempBucket->Insert(newEntry);//insert in full bucket -> creates automatically new bucket and inserts
                //cout << "Went to last bucket -- creating new bucket and inserting!" <<endl;
                return;
            }
        }
    }
}

AVLtree* HashTable::Find(string key){
    int bucketNum = Hash(key);
    Bucket* currBucket = this->listOfBuckets[bucketNum];
    for(int i = 0; i < currBucket->GetNumEntries(); i++){
        BucketEntry** entries = currBucket->GetBucketEntries();
        if(key.compare(*entries[i]->GetData()) == 0){
            return entries[i]->GetTree();
        }else if(i == currBucket->GetNumEntries() - 1){
            currBucket = currBucket->GetNextBucket();
            i = -1;
            if(currBucket == NULL){
                cout << "Unable to find Country / Disease -- Returning NULL!" << endl;
                return NULL;
            }
        }
    }
    return NULL;
}