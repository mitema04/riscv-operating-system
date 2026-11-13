//
// Created by os on 8/7/26.
//

#include "../inc/RiscV.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/TCB.hpp"
#include "../inc/Sem.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/Scheduler.hpp"
#include "../inc/Console.hpp"

void handleSupervisorTrap(uint64* regs) {

    volatile uint64 scause = RiscV::r_scause();
    volatile uint64 sepc = RiscV::r_sepc();
    volatile uint64 sstatus = RiscV::r_sstatus();

    if (scause == RiscV::SCAUSE_TIMER_INTERRUPT) {
        // prvo sklanjamo ovaj zahtev
        RiscV::mc_sip(RiscV::SIP_SSIP);

        // nema uvecavanje sepc jer je prekid!!!

        TCB::updateSleepingThreads();

        TCB::timeSliceCounter++;

        // ako je nas time slice istekao, moramo da predamo procesor
        if (TCB::running != nullptr && TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
    }

    else if (scause == RiscV::SCAUSE_EXTERNAL_INTERRUPT) {
        int irq = plic_claim();

        if (irq == CONSOLE_IRQ) {
            KernelConsole::handleInterrupt();
        }

        if (irq != 0) {
            plic_complete(irq);
        }
    }

    // ecall iz user ili supervisor moda
    else if (scause == RiscV::SCAUSE_ECALL_USER || scause == RiscV::SCAUSE_ECALL_SUPERVISOR) {
        // prelazimo na instrukciju posle ecall-a
        sepc += RiscV::INSTRUCTION_SIZE;

        // a0 sadrzi kod sistemskog poziva
        uint64 code = regs[RiscV::REG_A0];

        if (code == MEM_ALLOC_CODE) {
            uint64 blocks = regs[RiscV::REG_A1];
            void* result = MemoryAllocator::getInstance().mem_alloc(blocks);
            // povratna vrednost ide u a0
            regs[RiscV::REG_A0] = (uint64)result;
        }

        else if (code == MEM_FREE_CODE) {
            void* ptr = (void*)regs[RiscV::REG_A1];
            int result = MemoryAllocator::getInstance().mem_free(ptr);
            regs[RiscV::REG_A0] = (uint64)result;
        }

        else if (code == THREAD_CREATE_CODE) {
            thread_t* handle = (thread_t*)regs[RiscV::REG_A1];
            void (*start_routine)(void*) = (void(*)(void*))regs[RiscV::REG_A2];
            void* arg = (void*)regs[RiscV::REG_A3];
            void* stack = (void*)regs[RiscV::REG_A4];
            TCB* newThread = TCB::createThread(start_routine, arg, stack);

            if (newThread == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                *handle = (thread_t)newThread;
                Scheduler::put(newThread);
                regs[RiscV::REG_A0] = 0;
            }
        }

        else if (code == THREAD_EXIT_CODE) {
            regs[RiscV::REG_A0] = (uint64)TCB::exitThread();
        }

        else if (code == THREAD_DISPATCH_CODE) {
            TCB::dispatch();
        }

        else if (code == SEM_OPEN_CODE) {
            sem_t* handle = (sem_t*)regs[RiscV::REG_A1];
            unsigned initVal = (unsigned)regs[RiscV::REG_A2];
            Sem* sem = Sem::createSemaphore(initVal);

            if (sem == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                *handle = (sem_t)sem;
                regs[RiscV::REG_A0] = 0;
            }
        }

        else if (code == SEM_CLOSE_CODE) {
            sem_t handle = (sem_t)regs[RiscV::REG_A1];
            int result =Sem::freeSemaphore((Sem*)handle);
            regs[RiscV::REG_A0] = (uint64)result;
        }

        else if (code == SEM_WAIT_CODE) {
            sem_t handle = (sem_t)regs[RiscV::REG_A1];
            Sem* sem = (Sem*)handle;

            if (sem == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                int result = sem->wait();
                regs[RiscV::REG_A0] = (uint64)result;
            }
        }

        else if (code == SEM_SIGNAL_CODE) {
            sem_t handle = (sem_t)regs[RiscV::REG_A1];
            Sem* sem = (Sem*)handle;

            if (sem == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                sem->signal();
                regs[RiscV::REG_A0] = 0;
            }
        }

        else if (code == SEM_WAIT_N_CODE) {
            sem_t handle = (sem_t)regs[RiscV::REG_A1];
            Sem* sem = (Sem*)handle;
            unsigned n = (unsigned)regs[RiscV::REG_A2];

            if (sem == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                regs[RiscV::REG_A0] = (uint64)sem->waitN(n);
            }
        }

        else if (code == SEM_SIGNAL_N_CODE) {
            sem_t handle = (sem_t)regs[RiscV::REG_A1];
            Sem* sem = (Sem*)handle;
            unsigned n = (unsigned)regs[RiscV::REG_A2];

            if (sem == nullptr) {
                regs[RiscV::REG_A0] = (uint64)-1;
            }
            else {
                sem->signalN(n);
                regs[RiscV::REG_A0] = 0;
            }
        }

        else if (code == TIME_SLEEP_CODE) {
            time_t time = regs[RiscV::REG_A1];
            int ret = TCB::sleep(time);
            regs[RiscV::REG_A0] = (uint64)ret;
        }

        else if (code == CONSOLE_GETC_CODE) {
            char c = KernelConsole::getc();
            regs[RiscV::REG_A0] = (uint64)c;
        }

        else if (code == CONSOLE_PUTC_CODE) {
            char c = (char)regs[RiscV::REG_A1];
            KernelConsole::putc(c);
        }
    }

    RiscV::w_sstatus(sstatus);
    RiscV::w_sepc(sepc);
}


void RiscV::switchToUserMode() {
    __asm__ volatile("csrc sstatus, %0": : "r"(RiscV::SSTATUS_SPP));
    __asm__ volatile("csrs sstatus, %0": : "r"(RiscV::SSTATUS_SPIE));
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}