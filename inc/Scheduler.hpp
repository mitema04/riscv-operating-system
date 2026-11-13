//
// Created by os on 8/8/26.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../inc/List.hpp"

class TCB;

class Scheduler {
public:
    static TCB* get();
    static void put(TCB* tcb);
    static TCB* idleThread;
    static void setIdle(TCB* tcb);

private:
    static List<TCB> readyThreads;
};



#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
