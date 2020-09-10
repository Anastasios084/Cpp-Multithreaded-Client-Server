template<typename T>
#ifndef NODE_H
#define NODE_H
class LinkedListNode{
private:
    T item;
    LinkedListNode<T>* next;
public:
    LinkedListNode(T newitem);
    ~LinkedListNode();
    LinkedListNode<T>* GetNextNode();
    T GetItem();
    void SetNext(LinkedListNode<T>*);

};
#endif
// LinkedListNode::LinkedListNode(/* args */)
// {
// }

// LinkedListNode::~LinkedListNode()
// {
// }
