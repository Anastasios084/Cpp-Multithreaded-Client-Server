#include <iostream>
#include "LinkedListNode.h"
#include "Record.h"
using namespace std;
template<typename T>


LinkedListNode<T>::LinkedListNode(T item){
    this->item = item;
    this->next = NULL;
}
template<typename T>
LinkedListNode<T>::~LinkedListNode(){
    //cout << "Deleting Entry" << endl;
    delete this->item;
    delete this->next;

}

template<typename T>
LinkedListNode<T>* LinkedListNode<T>::GetNextNode(){
    return this->next;
}

template<typename T>
T LinkedListNode<T>::GetItem(){
    return this->item;
}

template<typename T>
void LinkedListNode<T>::SetNext(LinkedListNode<T>* node){
    this->next = node;
}

// template class LinkedListNode<int>;
// template class LinkedListNode<float>;
// template class LinkedListNode<char>;
// template class LinkedListNode<string>;
// template class LinkedListNode<Record>;
template class LinkedListNode<Record*>;
