//
// Created by os on 8/8/26.
//

#include "../inc/TCB.hpp"
#include "../inc/RiscV.hpp"
#include "../inc/Scheduler.hpp"
#include "../inc/syscall_c.hpp"

TCB* TCB::running = nullptr;
TCB* TCB::mainThread = nullptr;
TCB* TCB::userMainThread = nullptr;
TCB* TCB::threadToDelete = nullptr;

uint64 TCB::timeSliceCounter = 0;
List<TCB::SleepingThread> TCB::sleepingThreads;


void TCB::dispatch() {
    TCB* old = running;
    if (!old -> isFinished() && !old -> isBlocked()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    if (running != old && running != nullptr) {
        contextSwitch(&old -> context, &running -> context);
    }
}


int TCB::exitThread() {
    if (!running)
        return -1;

    //brisanje neke prethodno zavrsene niti, jer nit ne moze da obrise samu sebe
    cleanFinishedThread();

    // kad se userMain nit zavrsava, ona treba da probudi mainThread
    if (running == userMainThread && mainThread != nullptr) {
        mainThread->setBlocked(false);
        Scheduler::put(mainThread);
    }

    running -> setFinished(true);
    threadToDelete = running;
    dispatch();
    return 0;  // ovo cak ne bi trebalo nikad da se iskoristi niti da nam je bitno
}


TCB* TCB::createThread(Body body, void* arg, void* stack) {
    size_t blocks = (sizeof(TCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* mem = MemoryAllocator::getInstance().mem_alloc(blocks);

    if (mem == nullptr)
        return nullptr;

    TCB* tcb = (TCB*)mem;
    tcb -> body = body;
    tcb -> arg = arg;
    tcb -> stack = (uint64*)stack;
    tcb -> finished = false;
    tcb -> blocked = false;
    tcb -> timeSlice = DEFAULT_TIME_SLICE;

    if (body != nullptr) {
        tcb->context.ra = (uint64)&TCB::threadWrapper;
        tcb->context.sp = (uint64)stack + DEFAULT_STACK_SIZE;
    }
    else {
        tcb->context.ra = 0;
        tcb->context.sp = 0;
    }

    tcb->context.s0 = 0;
    tcb->context.s1 = 0;
    tcb->context.s2 = 0;
    tcb->context.s3 = 0;
    tcb->context.s4 = 0;
    tcb->context.s5 = 0;
    tcb->context.s6 = 0;
    tcb->context.s7 = 0;
    tcb->context.s8 = 0;
    tcb->context.s9 = 0;
    tcb->context.s10 = 0;
    tcb->context.s11 = 0;

    return tcb;
}

void TCB::threadWrapper() {
    RiscV::switchToUserMode();
    // i sad dolazimo na poziv ovoga kao da smo u user mode-u:
    running -> body(running -> arg);
    // kad se nit zavrsi, zelimo da predamo drugoj niti procesor
    // ovo je bukv pravi sistemski poziv
    thread_exit();
}


// zove se pravi dispatch iz syscall.c jer zelimo da ovaj body tretiramo kao svaki drugi user body
// radi uniformnosti
void TCB::idleBody(void*) {
    while (true) {
        thread_dispatch();
    }
}

// ono sto se nalazi u threadToDelete se oslobadja
void TCB::cleanFinishedThread() {
    if (threadToDelete != nullptr && threadToDelete != running) {
        TCB *old = threadToDelete;
        threadToDelete = nullptr;
        if (old->stack != nullptr) {
            MemoryAllocator::getInstance().mem_free(old->stack);
        }
        MemoryAllocator::getInstance().mem_free(old);
    }
}


int TCB::sleep(time_t time) {
    if (time == 0)
        return 0;

    size_t blocks = (sizeof(SleepingThread) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    SleepingThread* st = (SleepingThread*)MemoryAllocator::getInstance().mem_alloc(blocks);

    if (st == nullptr)
        return -1;

    st->thread = running;
    st->time = time;
    running -> setBlocked(true);
    sleepingThreads.addLast(st);
    dispatch();
    return 0;
}


// kad se desi timer tick, sve uspavane moraju da znaju da je proslo neko vreme
// mozda treba da se probude
void TCB::updateSleepingThreads() {
    SleepingThread* st = sleepingThreads.getFirst();
    while (st != nullptr) {
        SleepingThread* next = sleepingThreads.getNext(st);
        if (st->time > 0)
            st->time--;

        if (st->time == 0) {
            TCB* thread = st->thread;
            sleepingThreads.remove(st);
            thread->setBlocked(false);
            Scheduler::put(thread);
            MemoryAllocator::getInstance().mem_free(st);
        }
        st = next;
    }
}

// isto kao create thread, samo sto se ne preusmeravamo na wrapper da ne bismo usli u userMode
TCB* TCB::createKernelThread(Body body, void* arg, void* stack) {
    size_t blocks = (sizeof(TCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    void* mem = MemoryAllocator::getInstance().mem_alloc(blocks);

    if (mem == nullptr)
        return nullptr;

    TCB* tcb = (TCB*)mem;

    tcb->body = body;
    tcb->arg = arg;
    tcb->stack = (uint64*)stack;
    tcb->finished = false;
    tcb->blocked = false;

    tcb->timeSlice = DEFAULT_TIME_SLICE;

    tcb->context.ra = (uint64)body;
    tcb->context.sp = (uint64)stack + DEFAULT_STACK_SIZE;
    tcb->context.s0 = 0;
    tcb->context.s1 = 0;
    tcb->context.s2 = 0;
    tcb->context.s3 = 0;
    tcb->context.s4 = 0;
    tcb->context.s5 = 0;
    tcb->context.s6 = 0;
    tcb->context.s7 = 0;
    tcb->context.s8 = 0;
    tcb->context.s9 = 0;
    tcb->context.s10 = 0;
    tcb->context.s11 = 0;

    return tcb;
}

