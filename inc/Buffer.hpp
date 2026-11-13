//
// Created by os on 9/1/26.
//

#ifndef PROJECT_BASE_V1_1_BUFFER_HPP
#define PROJECT_BASE_V1_1_BUFFER_HPP

#include "Sem.hpp"

class KernelBuffer {
public:
    KernelBuffer(int capacity = 1024);
    ~KernelBuffer();
    void put(char c);
    char get();
    int size();
    int getCapacity() const;
    bool tryPutFromInterrupt(char c);

private:
    int capacity;
    char* buffer;
    int head;
    int tail;
    Sem* spaceAvailable;
    Sem* itemAvailable;
    Sem* mutexHead;
    Sem* mutexTail;
};

#endif //PROJECT_BASE_V1_1_BUFFER_HPP
