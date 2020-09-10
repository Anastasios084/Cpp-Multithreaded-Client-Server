#include "cyclicBuffer.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

cyclicBuffer::cyclicBuffer(int bufferSize){
    this->buffer = (int*)malloc(sizeof(int)*bufferSize);
    this->bufferSize = bufferSize;
    for(int i = 0; i < this->bufferSize;i++){
        this->buffer[i] = -1;
    }
    this->start = 0;
    this->end = 0;
}

cyclicBuffer::~cyclicBuffer(){
    free(this->buffer);
}

void cyclicBuffer::insert(int fd){
    if(this->end == this->start && this->buffer[this->end] != -1){
        cout << "Buffer is full ~ Aborting insertion." << endl;
        return;
    }
    this->buffer[this->end] = fd;
    this->end = this->end + 1;
    
    if(this->end == this->bufferSize){
        this->end = 0;
    }
    return;
}


int cyclicBuffer::pop(){
    if(this->end == this->start && this->buffer[this->end] == -1){
        cout << "Buffer is empty ~ Aborting pop." << endl;
        return -1;
    }
    int to_be_returned = this->buffer[this->start];
    this->buffer[this->start] = -1;
    this->start = this->start + 1;
    
    if(this->start == this->bufferSize){
        this->start = 0;
    }
    return to_be_returned;
}

int cyclicBuffer::empty(){
    if(this->end == this->start && this->buffer[this->end] == -1){
        return 1;
    }else{
        return 0;
    }
}

int cyclicBuffer::full(){
    if(this->end == this->start && this->buffer[this->end] != -1){
        return 1;
    }else{
        return 0;
    }
}