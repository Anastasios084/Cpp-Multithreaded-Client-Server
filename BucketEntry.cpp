#include <iostream>
#include "BucketEntry.h"

using namespace std;

BucketEntry::BucketEntry(string* data){
    this->data = data;
    this->tree = NULL;
}

BucketEntry::BucketEntry(string* data, AVLtree* tree){
    this->data = data;
    this->tree = tree;
}

BucketEntry::~BucketEntry(){
    delete this->tree;
}

string* BucketEntry::GetData(){
    return this->data;
}

AVLtree* BucketEntry::GetTree(){
    return this->tree;
}