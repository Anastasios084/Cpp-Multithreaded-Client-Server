#include "Bucket.h"

#ifndef HASHTABLE_H
#define HASHTABLE_H

class HashTable
{
private:
    // actually just hashtableNumOfEntries
    int diseaseHashtableNumOfEntries;
    Bucket** listOfBuckets;
public:
    HashTable(int);
    ~HashTable();
    int Hash(string stringToBeHashed);
    void Insert(Record*, string);
    AVLtree* Find(string);
    Bucket** GetListOfBuckets();
};

#endif