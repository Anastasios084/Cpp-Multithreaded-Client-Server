#include <iostream>
#include "AVLtree.h"
#include "Auxilliary.h"

using namespace std;

AVLtree::AVLtree(){
    this->firstNode = NULL;
    this->size = 0;
}

AVLtree::~AVLtree(){
    DestroyRecursive(this->firstNode);
    //cout << "TREE DESTROYED" << endl;
}

void AVLtree::DestroyRecursive(AVLnode* node){
    if(node){
        DestroyRecursive(node->GetLeftChild());
        DestroyRecursive(node->GetRightChild());
        delete node;
    }
}

AVLnode* AVLtree::GetRoot(){
    return this->firstNode;
}

int AVLtree::GetHeight(AVLnode* node){
    if(node == NULL){
        return 0;   
    }
    return node->GetHeight();
}

AVLnode* AVLtree::RightRotation(AVLnode* y){
    AVLnode* x = y->GetLeftChild();
    AVLnode* T2 = x->GetRightChild();
    
    x->SetRightChild(y);
    y->SetLeftChild(T2);

    y->SetHeight(max(GetHeight(y->GetLeftChild()), GetHeight(y->GetRightChild())) + 1);
    x->SetHeight(max(GetHeight(x->GetLeftChild()), GetHeight(x->GetRightChild())) + 1);
    //cout << "Succesful Right Rotation" << endl;
    return x;
}

AVLnode* AVLtree::LeftRotation(AVLnode* x){

    AVLnode* y = x->GetRightChild();
    if (y == NULL){
        cout << "Error" << endl;
    }
    AVLnode* T2 = y->GetLeftChild();


    y->SetLeftChild(x);
    x->SetRightChild(T2);
    x->SetHeight(max(GetHeight(x->GetLeftChild()), GetHeight(x->GetRightChild())) + 1);
    y->SetHeight(max(GetHeight(y->GetLeftChild()), GetHeight(y->GetRightChild())) + 1);

    //cout << "Succesful Left Rotation" << endl;
    return y;
}

int AVLtree::GetSize(){
    return this->size;
}

int AVLtree::GetBalance(AVLnode* node){
    if(node == NULL){
        return 0;
    }
    return (GetHeight(node->GetLeftChild()) - GetHeight(node->GetRightChild()));
}


void AVLtree::SetRoot(AVLnode* newRoot){
    this->firstNode = newRoot;
}

AVLnode* AVLtree::Insert(AVLnode* node, Record* newRec){
    //cout << "INSERTING" << endl;
    if(node == NULL){
        AVLnode* newNode = new AVLnode(newRec);
        size++;
        return newNode;
    }
    if(!(DateComparison(newRec->GetEntryDate(), node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate())) && !(DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate()))){ //aka ==
        //cout << "FOUND NODE WITH SAME DATE" << endl;
        node->GetRecords()->Insert(newRec);
        return node;
    }
    

    if (!(DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate()))){ 
        node->SetLeftChild(Insert(node->GetLeftChild(), newRec));
        //cout << "NEW LEFT CHILD" << endl;
    }else if (DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate())){
        node->SetRightChild(Insert(node->GetRightChild(), newRec));
       // cout << "NEW RIGHT CHILD" << endl;
    }

    node->SetHeight(1 + max(GetHeight(node->GetLeftChild()), GetHeight(node->GetRightChild())));
    //cout << node->GetHeight() << endl;
    int balance = GetBalance(node);
    
    //left left
    
    if(balance > 1 && !(DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate()))){
        //cout << "left left --- ";
    
        return RightRotation(node);
    }

    //right right
    if(balance < -1 && DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate())){
        
        //cout << "right right --- ";

        return LeftRotation(node);
    }

    //left right///////
    if(balance > 1 && DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate())){
        //cout << "left right --- ";
        node->SetLeftChild(LeftRotation(node->GetLeftChild()));

        return RightRotation(node);
    }

    //right left
    if(balance < -1 && !(DateComparison(node->GetRecords()->GetFirstNode()->GetItem()->GetEntryDate(), newRec->GetEntryDate()))){
        //cout << "right left --- ";
        node->SetRightChild(RightRotation(node->GetRightChild()));
        return LeftRotation(node);
    }
    //cout <<endl;
    return node;

}

