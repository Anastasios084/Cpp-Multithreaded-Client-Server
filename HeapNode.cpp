#include "HeapNode.h"

using namespace std;

HeapNode::HeapNode(){
    this->id = NULL;
    this->counter = 1;
    this->left = NULL;
    this->right = NULL;
    this->parent = NULL;
    this->lastInserted = NULL;
}

HeapNode::HeapNode(string* id){
    this->id = id;
    this->counter = 1;
    this->left = NULL;
    this->right = NULL;
    this->parent = NULL;
    this->lastInserted = NULL;
}


HeapNode::~HeapNode(){
}

void HeapNode::CounterUpdate(){
    this->counter = this->counter + 1;
    return;
}

int HeapNode::GetCounter(){
    return this->counter;
}

string* HeapNode::GetID(){
    return  this->id;
}

HeapNode* HeapNode::GetLeftChild(){
    return this->left;
}

HeapNode* HeapNode::GetRightChild(){
    return this->right;
}

HeapNode* HeapNode::GetParent(){
    return this->parent;
}

HeapNode* HeapNode::GetLastInserted(){
    return this->lastInserted;
}

void HeapNode::SetLeftChild(HeapNode* child){
    this->left = child;
    return;
}

void HeapNode::SetRightChild(HeapNode* child){
    this->right = child;
    return;
}

void HeapNode::SetParent(HeapNode* parent){
    this->parent = parent;
    return;
}

void HeapNode::SetCounter(int newCounter)
{
    this->counter = newCounter;
}

void HeapNode::SetID(string* newID){
    this->id = newID;
}

void HeapNode::SetLastInserted(HeapNode* last){
    this->lastInserted = last;
}
