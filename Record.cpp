#include <iostream>
#include "Record.h"

using namespace std;

Record::Record(){
    this->recordID = ' ';
    this->patientFirstName = ' ';
    this->patientLastName = ' '; 
    this->diseaseID = ' ';
    this->country = ' ';
    this->entryDate = ' ';
    this->exitDate = ' ';
    this->age = -1;
}
Record::Record(string recordID, string patientFirstName, string patientLastName, string diseaseID, string country, string entryDate, string exitDate, int age){
    this->recordID = recordID;
    this->patientFirstName = patientFirstName;
    this->patientLastName = patientLastName; 
    this->diseaseID = diseaseID;
    this->country = country;
    this->entryDate = entryDate;
    this->exitDate = exitDate;
    this->age = age;
}

Record::Record(const Record &rec){
    this->recordID = rec.recordID;
    this->patientFirstName = rec.patientFirstName;
    this->patientLastName = rec.patientLastName; 
    this->diseaseID = rec.diseaseID;
    this->country = rec.country;
    this->entryDate = rec.entryDate;
    this->exitDate = rec.exitDate;
    this->age = rec.age;

}

Record::~Record(){
    
}

string* Record::GetRecId(){
    return &(this->recordID);
}

string Record::GetPatientFirstName(){
    return this->patientFirstName;
}

string Record::GetPatientLastName(){
    return this->patientLastName;
}

string* Record::GetDiseaseID(){
    return &(this->diseaseID);
}

string* Record::GetCountry(){
    return &(this->country);
}

string Record::GetEntryDate(){
    return this->entryDate;
}

string* Record::GetExitDate(){
    return &this->exitDate;
}

void Record::SetExitDate(string* exit){
    this->exitDate = *exit;
}

int Record::GetAge(){
    return this->age;
}


bool operator==(Record me, Record other){
    return *me.GetRecId() == *other.GetRecId();
}

// bool operator>(Record me, Record other){
//     return me.GetRecId() > other.GetRecId();
// }

// bool operator<(Record me, Record other){
//     return me.GetRecId() < other.GetRecId();
// }