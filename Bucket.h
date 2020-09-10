#include <iostream>
#include "BucketEntry.h"

using namespace std;

#ifndef BUCKET_H
#define BUCKET_H


class Bucket
{
private:
    int BucketSize;
    int NumEntries;
    BucketEntry** bucket;
    Bucket* nextBucket;
public:
    static int size;
    Bucket();
    ~Bucket();

    Bucket* GetNextBucket();
    BucketEntry** GetBucketEntries();
    int GetNumEntries();
    int GetCapacity();
    void Insert(BucketEntry*);
};
#endif