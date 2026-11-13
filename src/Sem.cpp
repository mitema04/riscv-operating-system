//
// Created by os on 8/11/26.
//

#include "../inc/Sem.hpp"
#include "../inc/Scheduler.hpp"
#include "../inc/syscall_c.hpp"

Sem* Sem::createSemaphore(unsigned init) {
    size_t blocks = (sizeof(Sem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* mem = MemoryAllocator::getInstance().mem_alloc(blocks);
    if (mem == nullptr)
        return nullptr;
    Sem* sem = (Sem*)mem;
    sem -> val = init;
    sem -> blocked.initialize();
    return sem;
}

void Sem::block(unsigned n) {
    size_t blocksNeeded = (sizeof(BlockedThread) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* mem = MemoryAllocator::getInstance().mem_alloc(blocksNeeded);
    if (mem == nullptr)
        return;
    BlockedThread* bt = (BlockedThread*)mem;
    bt -> thread = TCB::running;
    bt -> requested = n;
    TCB::running -> setBlocked(true);
    blocked.addLast(bt);
    TCB::dispatch();
}

void Sem::deblock() {
    BlockedThread* bt = blocked.removeFirst();
    if (bt == nullptr)
        return;
    TCB* thr = bt -> thread;
    thr -> setBlocked(false);
    thr -> setWaitResult(0);
    Scheduler::put(thr);
    MemoryAllocator::getInstance().mem_free(bt);
}


int Sem::waitN(unsigned n) {
    if (val < (int)n) {
        TCB::running->setWaitResult(0);
        block(n);
        return TCB::running->getWaitResult();
    }
    val -= n;
    return 0;
}


int Sem::wait() {
    return waitN(1);
}


void Sem::signalN(unsigned n) {
    val += n;
    BlockedThread* bt = blocked.getFirst();
    while (bt != nullptr) {
        BlockedThread* next = blocked.getNext(bt);
        if (bt->requested <= (unsigned)val) {
            val -= bt -> requested;
            TCB* thr = bt -> thread;
            blocked.remove(bt);
            thr -> setBlocked(false);
            thr -> setWaitResult(0);
            Scheduler::put(thr);
            MemoryAllocator::getInstance().mem_free(bt);
        }
        bt = next;
    }
}

void Sem::signal() {
    signalN(1);
}

int Sem::freeSemaphore(Sem *sem) {
    if (sem == nullptr)
        return -1;
    BlockedThread* bt;
    while ((bt = sem -> blocked.removeFirst()) != nullptr) {
        bt -> thread -> setBlocked(false);
        bt -> thread -> setWaitResult(-1);
        Scheduler::put(bt -> thread);
        MemoryAllocator::getInstance().mem_free(bt);
    }
    MemoryAllocator::getInstance().mem_free(sem);
    return 0;
}


bool Sem::tryWait()
{
    if (val <= 0)
        return false;

    --val;
    return true;
}