//
// Created by os on 8/8/26.
//
#include "../inc/MemoryAllocator.hpp"
#include "../lib/hw.h"

MemoryAllocator::MemoryAllocator() {
    freeListHead = (Block*)HEAP_START_ADDR;
    freeListHead -> next = nullptr;
    freeListHead -> prev = nullptr;
    freeListHead -> isFree = true;
    freeListHead -> size = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR) / MEM_BLOCK_SIZE - HEADER_BLOCKS;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0)
        return nullptr;

    Block* current = freeListHead;
    bool found = false;

    while (!found) {
        if (current == nullptr)
            break;
        if (current -> size < size) {
            current = current->next;
        }
        else {
            found = true;
            // ovo char* je bilo obavezno!!! zato sto se tako kastuje u bajtove
            if (current -> size > size + HEADER_BLOCKS) {
                Block* newBlock = (Block*)((char*)current + (size + HEADER_BLOCKS) * MEM_BLOCK_SIZE);
                if (current -> prev != nullptr)
                    current -> prev -> next = newBlock;
                if (current -> next != nullptr)
                    current -> next -> prev = newBlock;
                newBlock -> next = current -> next;
                newBlock -> prev = current -> prev;
                newBlock -> size = current -> size - HEADER_BLOCKS - size;
                newBlock->isFree = true;
                // OVA LINIJA MORA OVDE:
                current -> size = size;
                if (current == freeListHead)
                    freeListHead = newBlock;
            }
            else {
                if (current -> prev != nullptr)
                    current -> prev -> next = current -> next;
                if (current -> next != nullptr)
                    current -> next -> prev = current -> prev;
                if (current == freeListHead)
                    freeListHead = current -> next;
            }
            current -> next = nullptr;
            current -> prev = nullptr;
            current -> isFree = false;
            return (char*)current + HEADER_BLOCKS * MEM_BLOCK_SIZE;
        }
    }
    return nullptr;
}

int MemoryAllocator::mem_free(void* ptr) {
    if (ptr == nullptr)
        return -1;

    Block* toFree = (Block*)((char*)ptr - HEADER_BLOCKS * MEM_BLOCK_SIZE);

    if (toFree->isFree)
        return -1;

    Block* temp = freeListHead;
    Block* previous = nullptr;
    while (temp != nullptr && (char*)temp < (char*)toFree) {
        previous = temp;
        temp = temp->next;
    }

    toFree->prev = previous;
    toFree->next = temp;

    if (previous != nullptr)
        previous->next = toFree;
    else
        freeListHead = toFree;

    if (temp != nullptr)
        temp->prev = toFree;

    toFree->isFree = true;

    // merge ide sad

    if (tryMergingFreeBlocks(previous, toFree)) {
        tryMergingFreeBlocks(previous, previous -> next);
    }
    else
        tryMergingFreeBlocks(toFree, toFree -> next);

    return 0;
}

bool MemoryAllocator::tryMergingFreeBlocks(Block* left, Block* right) {
    if (left == nullptr || right == nullptr)
        return false;

    char* leftAddress = (char*)left;
    char* rightAddress = (char*)right;
    if (leftAddress + left -> size * MEM_BLOCK_SIZE + HEADER_BLOCKS * MEM_BLOCK_SIZE == rightAddress) {
        left -> size += HEADER_BLOCKS + right -> size;
        left -> next = right -> next;
        if (right -> next)
            right -> next -> prev = left;
        return true;
    }
    return false;
}
