//
// Created by os on 8/8/26.
//

#ifndef PROJECT_BASE_V1_1_QUEUE_HPP
#define PROJECT_BASE_V1_1_QUEUE_HPP
#include "../inc/MemoryAllocator.hpp"

template<typename T>
class List {
public:
    void addLast(T* data);
    T* removeFirst();
    void initialize();
    T* getFirst() const;
    T* getNext(T* data) const;
    void remove(T* data);

private:
    struct Elem {
        T* data;
        Elem* next;
    };
    Elem* head = nullptr;
    Elem* tail = nullptr;
};


// morali smo ovde jer inace nije radilo, linker greska zbog nepostojanja tipova
template <typename T>
T* List<T>::removeFirst() {
    if (head == nullptr)
        return nullptr;
    Elem* toRemove = head;
    T* data = head -> data;
    head = head -> next;

    if (head == nullptr)
        tail = nullptr;

    MemoryAllocator::getInstance().mem_free(toRemove);
    return data;
}

template <typename T>
void List<T>::addLast(T* data) {
    size_t blocks = (sizeof(Elem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    Elem* newElem = (Elem*)MemoryAllocator::getInstance().mem_alloc(blocks);
    newElem -> data = data;
    newElem -> next = nullptr;
    if (head == nullptr) {
        head = newElem;
        tail = newElem;
    }
    else {
        tail -> next = newElem;
        tail = newElem;
    }
}

template <typename T>
void List<T>::initialize() {
    head = nullptr;
    tail = nullptr;
}

template<typename T>
T* List<T>::getFirst() const {
    return head ? head->data : nullptr;
}

template<typename T>
T* List<T>::getNext(T* data) const {
    Elem* current = head;
    while (current) {
        if (current->data == data)
            return current->next ? current->next->data : nullptr;
        current = current->next;
    }
    return nullptr;
}

template<typename T>
void List<T>::remove(T* data) {
    Elem* current = head;
    Elem* previous = nullptr;

    while (current) {
        if (current->data == data) {
            if (previous)
                previous->next = current->next;
            else
                head = current->next;

            if (tail == current)
                tail = previous;

            MemoryAllocator::getInstance().mem_free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

#endif //PROJECT_BASE_V1_1_QUEUE_HPP
