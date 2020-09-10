#include "Heap.h"
#include <string.h>
#include <assert.h>

using namespace std;

Heap::Heap(){
    this->lastInserted = NULL;
    this->root = NULL;
}

Heap::~Heap(){
    DestroyRecursive(this->root);
    //cout << "TREE DESTROYED" << endl;
}

void Heap::DestroyRecursive(HeapNode* node){
    if(node){
        DestroyRecursive(node->GetLeftChild());
        DestroyRecursive(node->GetRightChild());
        delete node;
    }
}

void Heap::Swap(HeapNode* a, HeapNode* b){
    string* tempID;////
    int tempCounter;

    tempID = a->GetID();//////

    tempCounter =  a->GetCounter();
    a->SetID(b->GetID());
    a->SetCounter(b->GetCounter());

    b->SetID(tempID);
    b->SetCounter(tempCounter);

    //delete tempID;
}

HeapNode* Heap::GetRoot(){
    return this->root;
}

//actual insertion
void Heap::Insert(string* id){
    
    HeapNode* node = new HeapNode(id);/////////////////////////////////////////////

    // If heap is empty insert to root
    if (this->root == NULL){
        this->root = node;
        this->lastInserted = node;
        //cout<< "CREATED ROOT" << endl;
        //cout << this->root->GetID() <<  "        Inside" <<endl;

        return;
    }

    // We will be finding the node to insert below.

    // Start with the current last node and move up as long as the
    // parent exists and the current node is its right child.

    HeapNode* currNode = this->lastInserted;

    //cout << *currNode->GetID() << "     Outside" <<  endl;
    
    while(currNode->GetParent() != NULL && currNode == currNode->GetParent()->GetRightChild()){
        currNode = currNode->GetParent();
    }

    if(currNode->GetParent() != NULL){
        if(currNode->GetParent()->GetRightChild() != NULL){
            // The parent has a right child. Attach the new node to
            // the leftmost node of the parent's right subtree.
            currNode = currNode->GetParent()->GetRightChild();
            while(currNode->GetLeftChild() != NULL){
                currNode = currNode->GetLeftChild();
            }
        }else{
            // The parent has no right child. This can only happen when
            // the last node is a right child. The new node can become
            // the right child.

            currNode = currNode->GetParent();
        }
    }else{
        // We have reached the root. The new node will be at a new level,
        // the left child of the current leftmost node.
        while(currNode->GetLeftChild() != NULL){
            currNode = currNode->GetLeftChild();
        }
    }

    // This is the node below which we will insert. It has either no
    // children or only a left child.
    //assert(currNode->GetRightChild() == NULL);

    //this->oldLastInserted = this->lastInserted;//??????????????????
    node->SetLastInserted(this->lastInserted);//???????????????????????????????

    this->lastInserted = node;
    if(currNode->GetLeftChild() == NULL){
        currNode->SetLeftChild(node);
    }else{
        currNode->SetRightChild(node);
    }
    
    node->SetParent(currNode);
    
    while(node->GetParent() != NULL && node->GetParent()->GetCounter() < node->GetCounter()){
        Swap(node->GetParent(), node);
        node = node->GetParent();// I GUESS IT WAS MISSING
    }
    //free(node);
}

//Use this to insert a new key (Updates the counter of an existing node or inserts a new node, aka updating the tree)
int Heap::Update(HeapNode* node, string* key){
    if(node == NULL){
        if(root == NULL){
            Insert(key);
        }
        return 0;
    }

    if(*node->GetID() == *key){
        //cout << *key << "  <-- ALREADY IN THE HEAP!" << endl;
        node->CounterUpdate();
        while(node->GetParent() != NULL && node->GetParent()->GetCounter() < node->GetCounter()){
            Swap(node->GetParent(), node);
        }
        return 1;
    }
    if( Update(node->GetLeftChild(), key) == 1){
        return 1;
    }
    if( Update(node->GetRightChild(), key) == 1){
        return 1;
    }

    // DIDN'T FIND THE KEY IN THE TREE, CREATING A NEW NODE AND INSERTING!
    if(node == root)
         Insert(key);
    return 0; 
}

void Heap::MaxHeapify(HeapNode* node){
    HeapNode* left = node->GetLeftChild(); 
    HeapNode* right = node->GetRightChild(); 
    HeapNode* biggest = node;
    if (left != NULL && left->GetCounter() > biggest->GetCounter()) 
        biggest = left; 
    if (right != NULL && right->GetCounter() > biggest->GetCounter()) 
        biggest = right; 
    if (biggest != node) 
    { 
        Swap(node, biggest); 
        MaxHeapify(biggest); 
    } 
}

string* Heap::ExtractMax(){

    if(this->root == NULL){
        cout << "EMPTY HEAP! RETURNING NULL..." << endl;
        return NULL;
    }

    // Getting the MAXIMUM value aka the root
    string* wanted;
    wanted = this->root->GetID();

    if(this->root == this->lastInserted){
        //free root and stuff
        this->root = NULL;
        this->lastInserted = NULL; // maybe not needed
        return wanted;
    }
    
    Swap(this->root, this->lastInserted);
    //free last inserted
    HeapNode* oldLastInserted = this->lastInserted->GetLastInserted();
    if(this->lastInserted->GetParent()->GetRightChild() == this->lastInserted){
        this->lastInserted->GetParent()->SetRightChild(NULL);
        delete this->lastInserted;
        

    }else{
        this->lastInserted->GetParent()->SetLeftChild(NULL);
        delete this->lastInserted;
        
    }
    this->lastInserted = oldLastInserted;
    //this->lastInserted  = this->oldLastInserted;//??????????????
    MaxHeapify(this->root);
    

    return wanted;
}