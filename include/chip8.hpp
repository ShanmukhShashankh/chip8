#pragma once
#include <types.h>

class Chip8 {
  public:
    Chip8();
    void CPUReset(); 
    void cycle();
    void updateTimers();
    bool loadROM(char const* filename);
    bool isSoundActive();

    BYTE keypad[16];
    BYTE screenData[32][64];
    bool drawFlag;

  private:
    void decode(WORD opcode);

    // Memory and Registers
    BYTE gameMemory[0x1000];
    BYTE registers[16];
    WORD addressI;
    WORD programCounter;
    WORD stack[16];
    BYTE stackPointer;

    // Timers
    BYTE delayTimer;
    BYTE soundTimer;

    // Font Data
    static const BYTE fontset[80];
};
