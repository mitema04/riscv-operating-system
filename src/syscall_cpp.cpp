//
// Created by os on 8/8/26.
//

#include "../inc/syscall_cpp.hpp"

void* operator new(size_t size){
    return mem_alloc(size);
}

void* operator new[](size_t size) {
    return mem_alloc(size);
}

void operator delete(void* ptr){
    mem_free(ptr);
}

void operator delete[](void* ptr) {
    mem_free(ptr);
}

Thread::Thread(void (*body)(void *), void *arg) {
    this -> body = body;
    this -> arg = arg;
    this -> myHandle = nullptr;
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

Thread::~Thread() = default;


void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread() {
    myHandle = nullptr;
    this -> body = &Thread::runWrapper;
    this -> arg = this;
}

void Thread::runWrapper(void* threadToRun) {
    Thread* thr = (Thread*)threadToRun;
    thr->run();
}

void Thread::run() {}


int Thread::sleep(time_t time) {
    return time_sleep(time);
}

PeriodicThread::PeriodicThread(time_t period)
    : Thread(&PeriodicThread::runWrapper, this), period(period) {}

void PeriodicThread::terminate() {
    period = 0;
}

void PeriodicThread::runWrapper(void* arg) {
    PeriodicThread* pt = (PeriodicThread*)arg;
    while (pt->period != 0) {
        pt->periodicActivation();
        if (pt->period != 0)
            sleep(pt->period);
    }
}

Semaphore::Semaphore(unsigned init) {
    myHandle = nullptr;
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

// ovako je najlakse da se korite unutra pozivi globalnih getc i putc
char Console::getc(){
    return ::getc();
}

void Console::putc(char c){
    ::putc(c);
}
