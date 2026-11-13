//
// Created by os on 8/8/26.
//

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "../inc/List.hpp"

class TCB{
public:
    using Body = void(*)(void*);

    struct Context {
        uint64 ra; // gde da se vrati nakon sto je predala procesor
        uint64 sp;
        uint64 s0;
        uint64 s1;
        uint64 s2;
        uint64 s3;
        uint64 s4;
        uint64 s5;
        uint64 s6;
        uint64 s7;
        uint64 s8;
        uint64 s9;
        uint64 s10;
        uint64 s11;
    };

    static TCB* createThread(Body body, void* arg, void* stack);
    static int exitThread();

    static TCB* running;
    static TCB* mainThread;
    static TCB* userMainThread;
    static TCB* threadToDelete;

    static void threadWrapper();
    bool isFinished() const { return finished;}
    void setFinished(const bool f) { finished = f; }
    static void dispatch();
    static void idleBody(void*);
    static void cleanFinishedThread();
    bool isBlocked() const { return blocked; }
    void setBlocked(const bool b) { blocked = b; }
    void setWaitResult(const int wR) { waitResult = wR; }
    int getWaitResult() const { return waitResult; }

    static uint64 timeSliceCounter;

    uint64 getTimeSlice() const { return timeSlice; }
    static int sleep(time_t time);
    static void updateSleepingThreads();
    static TCB* createKernelThread(Body body, void* arg, void* stack);

private:
    Body body = nullptr;
    void* arg = nullptr;
    uint64* stack = nullptr;
    Context context = {};
    bool finished = false;
    bool blocked = false;
    int waitResult = 0;

    uint64 timeSlice = DEFAULT_TIME_SLICE;

    struct SleepingThread {
        TCB* thread;
        time_t time;
    };
    static List<SleepingThread> sleepingThreads;
};


extern "C" void contextSwitch(TCB::Context* oldContext, TCB::Context* runningContext);

#endif //PROJECT_BASE_V1_1_TCB_HPP
