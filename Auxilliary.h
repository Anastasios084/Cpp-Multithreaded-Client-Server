#include <string>
#include <sstream>
#include "Record.h"
#include "AVLtree.h"
#include "Bucket.h"
#include "HashTable.h"
#include "Heap.h"
#include "LinkedList.h"

using namespace std;
//Comparison of two dates -- If Date1 is later than Date2 -> Returns TRUE.
bool DateComparison(string, string);
//Checks if two records has the same id
bool RecIdComparison(Record*, Record*);
//Initializes a Record
Record* InitializeRec(string);
//Return the MAX of two integers
int max(int, int);
void printAVLtree(AVLnode*);
//Takes as argument the number of  entries to be inserted into the buckets. Shows the sequencing of the buckets, number of buckets and some other useful info.
void BucketTest(int);
//Prints contents of hash table
void PrintHashTable(HashTable*, int);
//Prints Heap tree (kinda pretty print)
void PrintHeap(HeapNode*, int);
//Counts krousmata between date1 and date2
void Count(AVLnode*, int*, string*, string*);
void Count(AVLnode*, int*, string*, string*, string*);
//First query
void GlobalDiseaseStats(HashTable*, int, string*, string*);
//second query
int DiseaseFrequency(HashTable*, string, string*, string, string);
//third query
void TopkDiseases(LinkedList<Record*>* , int, string, string*, string*);
//fourth query
void TopkCountries(LinkedList<Record*>*, int, string, string*, string*);
//fifth query
//sixth query
Record* Find(LinkedList<Record*>*, string*);
//last query ;(
int NumCurrentPatients(LinkedList<Record*>*,string*);