//
// Created by os on 8/7/26.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP

#include "../lib/hw.h"

static const uint64 MEM_ALLOC_CODE       = 0x01;
static const uint64 MEM_FREE_CODE        = 0x02;

static const uint64 THREAD_CREATE_CODE   = 0x11;
static const uint64 THREAD_EXIT_CODE     = 0x12;
static const uint64 THREAD_DISPATCH_CODE = 0x13;

static const uint64 SEM_OPEN_CODE        = 0x21;
static const uint64 SEM_CLOSE_CODE       = 0x22;
static const uint64 SEM_WAIT_CODE        = 0x23;
static const uint64 SEM_SIGNAL_CODE      = 0x24;
static const uint64 SEM_WAIT_N_CODE      = 0x25;
static const uint64 SEM_SIGNAL_N_CODE    = 0x26;

static const uint64 TIME_SLEEP_CODE      = 0x31;

static const uint64 CONSOLE_GETC_CODE    = 0x41;
static const uint64 CONSOLE_PUTC_CODE    = 0x42;

void* mem_alloc(size_t size);
int mem_free(void* ptr);

class _thread;
typedef _thread* thread_t;
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();

class _sem;
typedef _sem* sem_t;
int sem_open (sem_t* handle, unsigned init);
int sem_close (sem_t handle);
int sem_wait (sem_t id);
int sem_signal (sem_t id);
int sem_wait_n(sem_t id, unsigned n);
int sem_signal_n(sem_t id, unsigned n);

typedef unsigned long time_t;
int time_sleep(time_t time);

const int EOF = -1;
char getc();
void putc(char c);
#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
