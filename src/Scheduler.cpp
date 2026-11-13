//
// Created by os on 8/8/26.
//
#include "../inc/Scheduler.hpp"


List<TCB> Scheduler::readyThreads;
TCB* Scheduler::idleThread;

TCB* Scheduler::get() {
    TCB* tcb = readyThreads.removeFirst();
    if (tcb){
        return tcb;
    }
    return idleThread;
}

void Scheduler::put(TCB* tcb) {
    // da ne bismo idleThread stavljali u listu spremnih
    if (tcb == idleThread)
        return;
    readyThreads.addLast(tcb);
}

void Scheduler::setIdle(TCB *tcb) {
    idleThread = tcb;
}
