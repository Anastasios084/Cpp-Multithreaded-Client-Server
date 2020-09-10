options = -ggdb3 -std=c++11

output: server.o cyclicBuffer.o client.o clientAuxilliary.o main.o worker.o auxilliaryShared.o auxilliaryWorker.o Record.o LinkedListNode.o LinkedList.o AVLnode.o AVLtree.o BucketEntry.o Bucket.o HashTable.o HeapNode.o Heap.o Auxilliary.o
	g++ $(options) server.o cyclicBuffer.o auxilliaryShared.o auxilliaryWorker.o Record.o LinkedListNode.o LinkedList.o AVLnode.o AVLtree.o BucketEntry.o Bucket.o HashTable.o HeapNode.o Heap.o Auxilliary.o -o whoServer -lpthread
	g++ $(options) client.o clientAuxilliary.o auxilliaryShared.o auxilliaryWorker.o Record.o LinkedListNode.o LinkedList.o AVLnode.o AVLtree.o BucketEntry.o Bucket.o HashTable.o HeapNode.o Heap.o Auxilliary.o -o whoClient -lpthread
	g++ $(options) main.o auxilliaryShared.o auxilliaryWorker.o Record.o LinkedListNode.o LinkedList.o AVLnode.o AVLtree.o BucketEntry.o Bucket.o HashTable.o HeapNode.o Heap.o Auxilliary.o -o master
	g++ $(options) worker.o auxilliaryShared.o auxilliaryWorker.o Record.o LinkedListNode.o LinkedList.o AVLnode.o AVLtree.o BucketEntry.o Bucket.o HashTable.o HeapNode.o Heap.o Auxilliary.o -o worker


server.o: server.cpp
	g++ $(options) -c server.cpp -lpthread

client.o: client.cpp
	g++ $(options) -c client.cpp -lpthread

cyclicBuffer.o: cyclicBuffer.cpp
	g++ $(options) -c cyclicBuffer.cpp

clientAuxilliary.o: clientAuxilliary.cpp
	g++ $(options) -c clientAuxilliary.cpp

main.o: main.cpp
	g++ $(options) -c main.cpp
	
worker.o: worker.cpp
	g++ $(options) -c worker.cpp

auxilliaryShared.o: auxilliaryShared.cpp
	g++ $(options) -c auxilliaryShared.cpp

auxilliaryWorker.o: auxilliaryWorker.cpp
	g++ $(options) -c auxilliaryWorker.cpp

Record.o: Record.cpp Record.h
	g++ $(options) -c Record.cpp

LinkedListNode.o: LinkedListNode.cpp LinkedListNode.h
	g++ $(options) -c LinkedListNode.cpp

LinkedList.o: LinkedList.cpp LinkedList.h
	g++ $(options) -c LinkedList.cpp

AVLnode.o: AVLnode.cpp AVLnode.h
	g++ $(options) -c AVLnode.cpp

AVLtree.o: AVLtree.cpp AVLtree.h
	g++ $(options) -c AVLtree.cpp

BucketEntry.o: BucketEntry.cpp BucketEntry.h
	g++ $(options) -c BucketEntry.cpp

Bucket.o: Bucket.cpp Bucket.h
	g++ $(options) -c Bucket.cpp

HashTable.o: HashTable.cpp HashTable.h
	g++ $(options) -c HashTable.cpp

HeapNode.o: HeapNode.cpp HeapNode.h
	g++ $(options) -c HeapNode.cpp

Heap.o: Heap.cpp Heap.h
	g++ $(options) -c Heap.cpp

Auxilliary.o: Auxilliary.cpp Auxilliary.h
	g++ $(options) -c Auxilliary.cpp

clean:
	rm *.o whoClient whoServer master worker
