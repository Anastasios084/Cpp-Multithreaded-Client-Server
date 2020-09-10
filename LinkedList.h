#include "LinkedListNode.h"

template<typename T>
#ifndef LIST_H
#define LIST_H
class LinkedList{
private:
    int counter;
    LinkedListNode<T>* firstNode;
public:
    LinkedList();
    LinkedList(T item);
    ~LinkedList();
    LinkedListNode<T>* GetFirstNode();
    void Insert(T);
    bool CheckAllIds(T);
};
#endif