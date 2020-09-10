#include <iostream>
#include "Bucket.h"
using namespace std;

Bucket::Bucket(){
    this->BucketSize = (size - 2*sizeof(int) - sizeof(BucketEntry**) - sizeof(Bucket*))/sizeof(BucketEntry*);
    this->NumEntries = 0;
    (this->bucket) = (BucketEntry**) malloc(this->BucketSize * sizeof(BucketEntry*));
    for(int i = 0; i < this->BucketSize; i++){
        this->bucket[i] = NULL;
    }
    this->nextBucket = NULL;
}

Bucket::~Bucket(){
    delete this->nextBucket;
    for(int i = 0; i < BucketSize; i++){
        delete this->bucket[i];
    }
    free(this->bucket);
}

Bucket* Bucket::GetNextBucket(){
    return this->nextBucket;
}

BucketEntry** Bucket::GetBucketEntries(){
    return this->bucket;
}

int Bucket::GetNumEntries(){
    return this->NumEntries;
}

int Bucket::GetCapacity(){
    return this->BucketSize;
}

void Bucket::Insert(BucketEntry* newEntry){
    if(this->NumEntries < this->BucketSize){//If entry fits, insert it
        this->bucket[this->NumEntries] = newEntry;
        this->NumEntries = this->NumEntries + 1;
    }else if (this->nextBucket == NULL){//else if next bucket is Null, which means if the last bucket is full, create a new one
        //maybe right size?
        Bucket* newBucket = new Bucket();
        this->nextBucket = newBucket;
        this->nextBucket->Insert(newEntry);
    }else{//if we are not on the last bucket, move on until we find it
        Bucket* tempBucket = this->nextBucket;
        while(tempBucket->GetNextBucket() != NULL){//Gets next bucket until we find the last one
            tempBucket = tempBucket->GetNextBucket();
        }
        tempBucket->Insert(newEntry); //try to insert on last block and start the procedure from the top.
    }
    return;
}