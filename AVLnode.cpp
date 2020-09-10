#include <iostream>
#include "Auxilliary.h"
#include "AVLnode.h"

AVLnode::AVLnode(){
    this->rec = NULL;
    this->right = NULL;
    this->left = NULL;
    this->height = 1;
}

AVLnode::AVLnode(Record* record){
    this->rec = new LinkedList<Record*>();
    this->rec->Insert(record);
    this->right = NULL;
    this->left = NULL;
    this->height = 1;
}

AVLnode::~AVLnode(){
    delete(this->rec);

}
AVLnode* AVLnode::GetRightChild(){
    return this->right;
}

AVLnode* AVLnode::GetLeftChild(){
    return this->left;
}

LinkedList<Record*>* AVLnode::GetRecords(){
    return this->rec;
}

int AVLnode::GetHeight(){
    return this->height;
}

void AVLnode::SetLeftChild(AVLnode* newNode){
    this->left = newNode;
}

void AVLnode::SetRightChild(AVLnode* newNode){
    this->right = newNode;
}

void AVLnode::SetHeight(int newHeight){
    this->height = newHeight;
}



//DateComparison return TRUE ONLY IF date1 > date2 || if date1 == date2 then date1>date2 => false and date2>date1 => false.
bool operator!=(AVLnode me, AVLnode other){
    return (DateComparison(me.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), other.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate()) && DateComparison(other.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), me.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate()));
    //probably right
}

bool operator>(AVLnode me, AVLnode other){
    //cout << DateComparison(me.GetEntryDate(), other.GetEntryDate()) << endl;
    return DateComparison(me.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), other.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate());
}

bool operator<(AVLnode me, AVLnode other){
    return DateComparison(other.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), me.GetRecords()->GetFirstNode()->GetItem()->GetEntryDate());
}
