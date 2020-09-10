#include <iostream>
#include "HeapNode.h"
//#include "Auxilliary.h"
#include "Record.h"

#ifndef HEAP_H
#define HEAP_H

using namespace std;

class Heap
{
private:
    HeapNode* root;
    HeapNode* lastInserted;
public:
    Heap(/* args */);
    ~Heap();
    void DestroyRecursive(HeapNode*);

    HeapNode* GetRoot();
    void Swap(HeapNode*, HeapNode*);
    //Creates and Inserts new node to the heap
    void Insert(string*);
    //Checks if key is CONTAINED in the heap and updates the counter of the node, else INSERTS key.
    int Update(HeapNode* ,string*);
    // Returns Max ID
    string* ExtractMax();
    void MaxHeapify(HeapNode*);
};

#endif