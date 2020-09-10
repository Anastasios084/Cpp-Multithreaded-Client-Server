#include <iostream>

#ifndef HEAPNODE_H
#define HEAPNODE_H

using namespace std;

class HeapNode
{
private:
    string* id;
    int counter;
    HeapNode* parent;
    HeapNode* left;
    HeapNode* right;
    HeapNode* lastInserted;
    
public:
    HeapNode();
    HeapNode(string*);
    ~HeapNode();

    void CounterUpdate();

    int GetCounter();
    string* GetID();

    HeapNode* GetLeftChild();
    HeapNode* GetRightChild();
    HeapNode* GetParent();
    HeapNode* GetLastInserted();

    void SetLeftChild(HeapNode*);
    void SetRightChild(HeapNode*);
    void SetParent(HeapNode*);
    void SetCounter(int);
    void SetID(string*);
    void SetLastInserted(HeapNode*);

};

#endif