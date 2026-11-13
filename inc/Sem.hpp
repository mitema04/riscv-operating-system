//
// Created by os on 8/11/26.
//

#ifndef PROJECT_BASE_V1_1_SEMAPHORE_HPP
#define PROJECT_BASE_V1_1_SEMAPHORE_HPP
#include "../inc/List.hpp"
#include "../inc/TCB.hpp"

class Sem {
public:
    static Sem* createSemaphore(unsigned init);
    static int freeSemaphore(Sem* sem);
    int wait();
    void signal();
    int waitN(unsigned n);
    void signalN(unsigned n);
    bool tryWait();


private:
    struct BlockedThread {
        TCB* thread;
        unsigned requested;
    };

    void block(unsigned n);
    void deblock();
    int val = 1;
    int waitResult = 0;
    List<BlockedThread> blocked;
};

#endif //PROJECT_BASE_V1_1_SEMAPHORE_HPP
