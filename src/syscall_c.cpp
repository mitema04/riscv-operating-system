//
// Created by os on 8/7/26.
//

#include "../inc/syscall_c.hpp"
#include "../inc/MemoryAllocator.hpp"

void* mem_alloc(size_t size){
    size_t blocks_num = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    __asm__ volatile ("mv a1, %[bn]" : : [bn] "r" (blocks_num));
    uint64 code = MEM_ALLOC_CODE;
    __asm__ volatile ("mv a0, %[cd]" : : [cd] "r" (code));
    __asm__ volatile ("ecall");
    void* result;
    __asm__ volatile ("mv %[res], a0": [res] "=r"(result));
    return result;
}


int mem_free(void* ptr) {
    __asm__ volatile ("mv a1, %[p]": : [p] "r" (ptr));
    uint64 code = MEM_FREE_CODE;
    __asm__ volatile ("mv a0, %[cd]" : : [cd] "r" (code));
    __asm__ volatile ("ecall");
    int result;
    __asm__ volatile ("mv %[res], a0": [res] "=r"(result));
    return result;
}


int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack == nullptr)
        return -1;
    uint64 code = THREAD_CREATE_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "mv a2, %[s]\n"
       "mv a3, %[a]\n"
       "mv a4, %[st]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle),
         [s] "r" (start_routine),
         [a] "r" (arg),
         [st] "r" (stack)
       : "a0", "a1", "a2", "a3", "a4", "memory");

    if (result != 0) {
        mem_free(stack);
    }

    return result;
}


int thread_exit() {
    uint64 code = THREAD_EXIT_CODE;
    __asm__ volatile ("mv a0, %[cd]" : : [cd] "r" (code));
    __asm__ volatile ("ecall");
    int result;
    __asm__ volatile ("mv %[res], a0": [res] "=r"(result));
    return result;
}


void thread_dispatch() {
    uint64 code = THREAD_DISPATCH_CODE;
    __asm__ volatile ("mv a0, %[cd]" : : [cd] "r" (code));
    __asm__ volatile ("ecall");
}


int sem_open(sem_t *handle, unsigned init) {
    uint64 code = SEM_OPEN_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "mv a2, %[in]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle),
         [in] "r" (init)
       : "a0", "a1", "a2", "memory");
    return result;
}


int sem_close(sem_t handle) {
    uint64 code = SEM_CLOSE_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle)
       : "a0", "a1", "memory");

    return result;
}


int sem_wait(sem_t handle) {
    uint64 code = SEM_WAIT_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle)
       : "a0", "a1", "memory");

    return result;
}


int sem_signal(sem_t handle) {
    uint64 code = SEM_SIGNAL_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle)
       : "a0", "a1", "memory");

    return result;
}


int sem_wait_n(sem_t handle, unsigned n) {
    uint64 code = SEM_WAIT_N_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "mv a2, %[n]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle),
         [n] "r" (n)
       : "a0", "a1", "a2", "memory");

    return result;
}


int sem_signal_n(sem_t handle, unsigned n) {
    uint64 code = SEM_SIGNAL_N_CODE;
    int result;
    __asm__ volatile (
       "mv a0, %[cd]\n"
       "mv a1, %[h]\n"
       "mv a2, %[n]\n"
       "ecall\n"
       "mv %[res], a0\n"
       : [res] "=r" (result)
       : [cd] "r" (code),
         [h] "r" (handle),
         [n] "r" (n)
       : "a0", "a1", "a2", "memory");

    return result;
}


int time_sleep(time_t time) {
    uint64 code = TIME_SLEEP_CODE;
    uint64 ret;
    __asm__ volatile(
        "mv a0, %[cd]\n"
        "mv a1, %[tm]\n"
        "ecall\n"
        "mv %[ret], a0\n"
        : [ret] "=r" (ret)
        : [cd] "r" (code),
          [tm] "r" (time)
        : "a0", "a1", "memory");

    return (int)ret;
}


void putc(char c) {
    uint64 code = CONSOLE_PUTC_CODE;
    __asm__ volatile (
        "mv a0, %[cd]\n"
        "mv a1, %[ch]\n"
        "ecall\n"
        :
        : [cd] "r" (code),
          [ch] "r" (c)
        : "a0", "a1", "memory");
}


char getc() {
    uint64 code = CONSOLE_GETC_CODE;
    uint64 ret;
    __asm__ volatile (
        "mv a0, %[cd]\n"
        "ecall\n"
        "mv %[res], a0\n"
        : [res] "=r" (ret)
        : [cd] "r" (code)
        : "a0", "memory");

    return (char)ret;
}

