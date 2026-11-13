//
// Created by os on 9/1/26.
//


#include "../inc/Buffer.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/Sem.hpp"


KernelBuffer::KernelBuffer(int capacity)
    : capacity(capacity > 0 ? capacity : 1024),
      buffer(nullptr),
      head(0),
      tail(0),
      spaceAvailable(nullptr),
      itemAvailable(nullptr),
      mutexHead(nullptr),
      mutexTail(nullptr){

    size_t blocks = (this -> capacity + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    buffer = (char*)MemoryAllocator::getInstance().mem_alloc(blocks);
    spaceAvailable = Sem::createSemaphore(this->capacity);
    itemAvailable = Sem::createSemaphore(0);
    mutexHead = Sem::createSemaphore(1);
    mutexTail = Sem::createSemaphore(1);
}


KernelBuffer::~KernelBuffer(){
    Sem::freeSemaphore(mutexTail);
    Sem::freeSemaphore(mutexHead);
    Sem::freeSemaphore(itemAvailable);
    Sem::freeSemaphore(spaceAvailable);

    if (buffer != nullptr) {
        MemoryAllocator::getInstance().mem_free(buffer);
        buffer = nullptr;
    }
}

void KernelBuffer::put(char c){
    spaceAvailable -> wait();
    mutexTail -> wait();
    buffer[tail] = c;
    tail = (tail + 1) % capacity;
    mutexTail -> signal();
    itemAvailable -> signal();
}

bool KernelBuffer::tryPutFromInterrupt(char c){

    // ako mozemo da prodjemo kroz semafor odmah, prolazimo
    // svakako je ovo interrupt i svakako je ova trenutna nit
    // jedina koja i moze da puni input buffer
    // ne smemo da se blokiramo i cekamo ako ne mozemo da stavimo nesto u buffer
    if (!spaceAvailable -> tryWait())
        return false;

    buffer[tail] = c;
    tail = (tail + 1) % capacity;

    itemAvailable -> signal();

    return true;
}

char KernelBuffer::get(){
    itemAvailable -> wait();
    mutexHead -> wait();
    char c = buffer[head];
    head = (head + 1) % capacity;
    mutexHead -> signal();
    spaceAvailable -> signal();
    return c;
}

int KernelBuffer::size(){
    int n;
    mutexHead -> wait();
    mutexTail -> wait();

    if (tail >= head)
        n = tail - head;
    else
        n = capacity - (head - tail);

    mutexTail -> signal();
    mutexHead -> signal();
    return n;
}

int KernelBuffer::getCapacity() const{
    return capacity;
}