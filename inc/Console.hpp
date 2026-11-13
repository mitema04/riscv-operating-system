//
// Created by os on 9/1/26.
//

#ifndef PROJECT_BASE_V1_1_CONSOLE_HPP
#define PROJECT_BASE_V1_1_CONSOLE_HPP

#include "Buffer.hpp"
#include "TCB.hpp"

class KernelConsole {
public:
    static void initialize();

    static void putc(char c);
    static char getc();

    static void putcHandler(void*);
    static void handleInterrupt();

    static void flush();
    static void shutdown();

private:
    static KernelBuffer* outputBuffer;
    static KernelBuffer* inputBuffer;
    static volatile uint64 pendingOutput;
    static volatile bool shutdownRequested;
    static volatile bool handlerStopped;
};

#endif //PROJECT_BASE_V1_1_CONSOLE_HPP
