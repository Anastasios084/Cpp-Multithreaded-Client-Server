#include <iostream>

// char* READnamedPipe; // PARENT WRITES -> CHILD READS
// char* WRITEnamedPipe; // CHILD WRITES -> PARENT READS
// int bufferSize; //bufferSize
// char* input_dir;
// char* buffer;


class Record;
class HashTable;

template <typename DataType>
class LinkedList;
template <typename DataType>
class LinkedListNode;


using namespace std;

string** initializeCountries(string* countries, int* length);
Record* InitializeRec2(string line, LinkedList<Record*>* recs, string* date, string*);


LinkedList<Record*>* openFilesFromCountryDirsSorted(string**, int,  string**,char* ,HashTable**, HashTable**,int,char*, int);
string** getDates(string* country, char* input_dir,int* size);

int createStatistics(Record*, string*);

int organizeAndSendStats(string*,string*,string*, int*, int, int,char*,int);

string topkAgeRanges(int k, string* country, string* disease, string* date1, string* date2, HashTable* countryHashTable);
string numPatientsAdmissions(string* disease, string* date1, string* date2, string* country, HashTable* diseaseHashTable);
string numPatientsDischarges(string* disease, string* date1, string* date2, string* country, HashTable* diseaseHashTable);

void updateRecords(string** country, int numCountries, string** latestDatesPerCountry,char* inputDirr, LinkedList<Record*>* recList, HashTable** diseaseHashTable, HashTable** countryHashTable, int WRITEfd, char* buffer, int bufferSize);
