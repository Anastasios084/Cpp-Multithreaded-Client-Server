#include <iostream>
#include "LinkedList.h"
#include "Record.h"
#include "Bucket.h"
using namespace std;

template<typename T>
//TESTING to see that it works
LinkedList<T>::LinkedList(){
    this->counter = 0;
    this->firstNode = NULL;
}

template<typename T>
LinkedList<T>::LinkedList(T item){
    this->counter = 1;
    LinkedListNode<T>* newNode = new LinkedListNode<T>(item);
    this->firstNode = newNode;
}
template<typename T>
LinkedList<T>::~LinkedList(){
    delete this->firstNode;
    //cout << "Deleted List" << endl;
}

template<typename T>
LinkedListNode<T>* LinkedList<T>::GetFirstNode(){
    return this->firstNode;
}

template<typename T>
void LinkedList<T>::Insert(T item){
    
    LinkedListNode<T> *newNode = new LinkedListNode<T>(item);
    newNode->SetNext(this->firstNode);
    this->firstNode = newNode;

    this->counter = (this->counter) + 1;

    return;
}

template<typename T>
bool LinkedList<T>::CheckAllIds(T newItem){
    LinkedListNode<T> *currentNode = this->firstNode;
    while(currentNode != NULL){
        if(*currentNode->GetItem() == *newItem){
            return false;
        }
        currentNode = currentNode->GetNextNode();
    }
    return true;
}


// template class LinkedList<int>;
// template class LinkedList<float>;
// template class LinkedList<char>;
// template class LinkedList<string>;
template class LinkedList<Record*>;
