#include "../inc/Console.hpp"
#include "../lib/hw.h"

KernelBuffer* KernelConsole::outputBuffer = nullptr;
KernelBuffer* KernelConsole::inputBuffer = nullptr;

// koliko znakova imamo za ispis, a jos ih nismo dali hardveru:
volatile uint64 KernelConsole::pendingOutput = 0;

volatile bool KernelConsole::shutdownRequested = false;
volatile bool KernelConsole::handlerStopped = false;

void KernelConsole::initialize(){
    // ovo ce biti zapravo 2 sistemska poziva
    outputBuffer = new KernelBuffer(1024);
    inputBuffer = new KernelBuffer(1024);
}

void KernelConsole::putc(char c){
    outputBuffer -> put(c);
    ++pendingOutput;
}

char KernelConsole::getc(){
    return inputBuffer->get();
}


// ovo je telo putc thread-a koji stalno radi
void KernelConsole::putcHandler(void*)
{
    while (true) {
        char c = outputBuffer->get();
        // shutdown je ubacio jedan lazni znak kojim se prolazi get i sad se konacno gasi ova nit
        if (shutdownRequested) {
            handlerStopped = true;
            TCB::exitThread();
            return;
        }

        // busy wait, dok nije spreman uredjaj, cekamo
        while ((*((volatile uint8*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) == 0) {}
        *((volatile uint8*)CONSOLE_TX_DATA) = (uint8)c;
        --pendingOutput;
    }
}

void KernelConsole::handleInterrupt(){
    uint8 status = *((volatile uint8*)CONSOLE_STATUS);

    while (status & CONSOLE_RX_STATUS_BIT) {
        char c = (char)(*((volatile uint8*)CONSOLE_RX_DATA));
        // pokusamo da ubacimo znak u buffer
        inputBuffer -> tryPutFromInterrupt(c);
        status = *((volatile uint8*)CONSOLE_STATUS);
    }
}

// ovo ce izvrsavati main, ako jos postoji znakova za izlaz a sve ostale niti su se zavrsile
// moramo da pustimo putcThread da uradi svoje
void KernelConsole::flush(){
    while (pendingOutput != 0) {
        TCB::dispatch();
    }
}

void KernelConsole::shutdown(){
    flush();
    shutdownRequested = true;
    outputBuffer->put('\0');
    // cekamo da handelr obradi ovo i ugasi se
    while (!handlerStopped) {
        TCB::dispatch();
    }

    // ovim se apravo brise putc Thread
    TCB::cleanFinishedThread();

    delete outputBuffer;
    outputBuffer = nullptr;

    delete inputBuffer;
    inputBuffer = nullptr;
}