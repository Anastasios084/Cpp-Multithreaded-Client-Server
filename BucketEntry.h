#include <string>
#include "AVLtree.h"
using namespace std;

#ifndef BUCKETENTRY_H
#define BUCKETENTRY_H

class BucketEntry
{
private:
    string* data; //maybe need to free
    AVLtree* tree;
public:
    BucketEntry(string*);
    BucketEntry(string*, AVLtree*);
    ~BucketEntry();

    string* GetData();
    AVLtree* GetTree();
};

#endif