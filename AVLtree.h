#include "AVLnode.h"

#ifndef AVLTREE_H
#define AVLTREE_H
class AVLtree{
    private:
        AVLnode* firstNode;
        int size;
    public:
        AVLtree();
        ~AVLtree(); 
        void DestroyRecursive(AVLnode*);

        AVLnode* GetRoot();
        int GetHeight(AVLnode*);
        int GetSize();
        AVLnode* RightRotation(AVLnode* );
        AVLnode* LeftRotation(AVLnode* );
        void SetRoot(AVLnode* );
        int GetBalance(AVLnode* );
       
        AVLnode* Insert(AVLnode* node,Record* newRec);
};

#endif