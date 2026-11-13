//
// Created by os on 8/7/26.
//

#include "../inc/RiscV.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/TCB.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/Scheduler.hpp"
#include "../inc/Console.hpp"

extern void userMain();

void userMainWrapper(void*) {
    userMain();
}


int main() {

    RiscV::w_stvec((uint64)&supervisorTrap);
    // ovo radimo da bismo dozvolili prekide od konzole i tajmera
    RiscV::w_sie(RiscV::SIE_SSIE | RiscV::SIE_SEIE);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);
    TCB::mainThread = TCB::running;

    KernelConsole::initialize();

    size_t putcBlocks = (DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* putcStack = MemoryAllocator::getInstance().mem_alloc(putcBlocks);
    TCB* putcThread = TCB::createKernelThread(KernelConsole::putcHandler, nullptr, putcStack);
    Scheduler::put(putcThread);

    size_t idleBlocks = (DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* idleStack = MemoryAllocator::getInstance().mem_alloc(idleBlocks);
    TCB* idle = TCB::createThread(TCB::idleBody,nullptr,idleStack);
    Scheduler::setIdle(idle);

    size_t userBlocks = (DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* userStack = MemoryAllocator::getInstance().mem_alloc(userBlocks);
    TCB* user = TCB::createThread(userMainWrapper,nullptr, userStack);
    TCB::userMainThread = user;
    Scheduler::put(user);

    TCB::mainThread->setBlocked(true);
    thread_dispatch();

    TCB::cleanFinishedThread();
    KernelConsole::shutdown();

    Scheduler::setIdle(nullptr);
    MemoryAllocator::getInstance().mem_free(idleStack);
    MemoryAllocator::getInstance().mem_free(idle);

    // user i putcThread se preko thread oslobode, idle to nikad ne uradi

    TCB* mainTCB = TCB::running;
    TCB::running = nullptr;
    MemoryAllocator::getInstance().mem_free(mainTCB);
    return 0;

}