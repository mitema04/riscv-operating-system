//
// Created by os on 8/8/26.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    static MemoryAllocator& getInstance() {
        static MemoryAllocator instance;
        return instance;
    }
    void* mem_alloc(size_t size);
    int mem_free(void* ptr);

private:
    MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    struct Block {
        size_t size;
        Block* next;
        Block* prev;
        bool isFree;
    };

    Block* freeListHead;
    static const size_t HEADER_BLOCKS = (sizeof(Block) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    static bool tryMergingFreeBlocks(Block* left, Block* right);

};



#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
