//
// Created by os on 8/7/26.
//

#ifndef PROJECT_BASE_V1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_RISCV_HPP

#include "../lib/hw.h"
class RiscV {
public:

    static const uint64 SIP_SSIP = (1UL << 1);
    static const uint64 SIE_SSIE = (1UL << 1);
    static const uint64 SIE_SEIE = (1UL << 9);
    static const uint64 SSTATUS_SPIE = (1UL << 5);
    static const uint64 SSTATUS_SPP = (1UL << 8);

    static const uint64 SCAUSE_TIMER_INTERRUPT = 0x8000000000000001UL;
    static const uint64 SCAUSE_EXTERNAL_INTERRUPT = 0x8000000000000009UL;
    static const uint64 SCAUSE_ECALL_USER = 8;
    static const uint64 SCAUSE_ECALL_SUPERVISOR = 9;

    static const int REG_A0 = 10;
    static const int REG_A1 = 11;
    static const int REG_A2 = 12;
    static const int REG_A3 = 13;
    static const int REG_A4 = 14;

    static const uint64 INSTRUCTION_SIZE = 4;

    static void switchToUserMode();

    static uint64 r_sstatus();
    static void w_sstatus(uint64 value);

    static uint64 r_sip();
    static void w_sip(uint64 value);

    static uint64 r_sie();
    static void w_sie(uint64 value);

    static uint64 r_sscratch();
    static void w_sscratch(uint64 value);

    static uint64 r_sepc();
    static void w_sepc(uint64 value);

    static uint64 r_stvec();
    static void w_stvec(uint64 value);

    static uint64 r_scause();

    static void mc_sip(uint64 mask);

};

extern "C" void supervisorTrap();
extern "C" void handleSupervisorTrap(uint64* regs);

inline uint64 RiscV::r_sstatus() {
    uint64 value;
    __asm__ volatile("csrr %[val], sstatus" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_sstatus(uint64 value) {
    __asm__ volatile ("csrw sstatus, %[val]" : : [val] "r" (value));
}

inline uint64 RiscV::r_sip() {
    uint64 value;
    __asm__ volatile("csrr %[val], sip" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_sip(uint64 value) {
    __asm__ volatile ("csrw sip, %[val]" : : [val] "r" (value));
}

inline uint64 RiscV::r_sie() {
    uint64 value;
    __asm__ volatile("csrr %[val], sie" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_sie(uint64 value) {
    __asm__ volatile ("csrw sie, %[val]" : : [val] "r" (value));
}

inline uint64 RiscV::r_sscratch() {
    uint64 value;
    __asm__ volatile("csrr %[val], sscratch" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_sscratch(uint64 value) {
    __asm__ volatile ("csrw sscratch, %[val]" : : [val] "r" (value));
}

inline uint64 RiscV::r_sepc() {
    uint64 value;
    __asm__ volatile("csrr %[val], sepc" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_sepc(uint64 value) {
    __asm__ volatile ("csrw sepc, %[val]" : : [val] "r" (value));
}

inline uint64 RiscV::r_stvec() {
    uint64 value;
    __asm__ volatile("csrr %[val], stvec" : [val] "=r" (value));
    return value;
}

inline void RiscV::w_stvec(uint64 value) {
    __asm__ volatile ("csrw stvec, %[val]" : : [val] "r" (value));
}


// nece nam trebati write za scause
inline uint64 RiscV::r_scause() {
    uint64 value;
    __asm__ volatile("csrr %[val], scause" : [val] "=r" (value));
    return value;
}

inline void RiscV::mc_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %[mask]" : : [mask] "r" (mask));
}

#endif //PROJECT_BASE_V1_1_RISCV_HPP
