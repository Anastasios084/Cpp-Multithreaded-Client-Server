#include <string>
#include <iostream>
#ifndef RECORD_H
#define RECORD_H

using namespace std;
class Record{
private:
    string recordID;
    string patientFirstName;
    string patientLastName; 
    string diseaseID;
    string country;
    string entryDate;
    string exitDate;
    int age;
public:
    Record();
    Record(const Record &rec);
    Record(string, string , string, string, string, string, string,int);
    ~Record();
    string* GetRecId();
    string GetPatientFirstName();
    string GetPatientLastName();
    string* GetDiseaseID();
    string* GetCountry();
    string GetEntryDate();
    string* GetExitDate();
    int GetAge();
    void SetExitDate(string*);
    
    friend bool operator==(Record me, Record other);
    // friend bool operator>(Record me, Record other);
    // friend bool operator<(Record me, Record other);

};


#endif