
#ifndef CYCLICBUFFER_H
#define CYCLICBUFFER_H
class cyclicBuffer{
    private:
        int bufferSize;
        int* buffer;
        int start;
        int end;
    public:
        cyclicBuffer(int);
        ~cyclicBuffer();
        void insert(int);
        int pop();
        int empty();
        int full();
};

#endif