#include "Record.h"
//#include "LinkedList.h"

template <typename DataType>
class LinkedList;

#ifndef AVLNODE_H
#define AVLNODE_H
class AVLnode{
    private:
        LinkedList<Record*>* rec;
        AVLnode* right;
        AVLnode* left;
        int height;
    public:
        AVLnode();
        AVLnode(Record* rec);
        ~AVLnode();

        AVLnode* GetRightChild();
        AVLnode* GetLeftChild();
        int GetHeight();
        
        LinkedList<Record*>* GetRecords();

        void SetLeftChild(AVLnode* newNode);
        void SetRightChild(AVLnode* newNode);
        void SetHeight(int);

        void Insert(Record* rec);//?

        friend bool operator==(AVLnode me, AVLnode other);
        friend bool operator>(AVLnode me, AVLnode other);
        friend bool operator<(AVLnode me, AVLnode other);

};

#endif