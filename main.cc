#include "types.h"
#include <cstdio>
#include <cstring>
#include <iostream>

class Chip8 {
  BYTE gameMemory[0x1000]; // 0xFFF <=> 4096 bytes of memory
  BYTE registers[16];      // V0 to VF
  WORD addressI;           // 16 bit Address Register / Index
  WORD programCounter;     // 16 bit Program Counter
  WORD stack[16];          // Call Stack
  BYTE stackPointer = 0;   // Self Explanatory jimmy
  BYTE screenData[64][32]; // 64x32 resolution display
  BYTE delayTimer;
  BYTE soundTimer;
  BYTE keypad[16];
  WORD opcode;

public:
  void CPUReset() {
    addressI = 0;
    programCounter = 0x200;
    memset(registers, 0, sizeof(registers));

    FILE *in;
    in = fopen("/mnt/BambooCopter/Games/INVADERS", "rb");
    fread(&gameMemory[0x200], 0xFFF, 1, in);
    fclose(in);
  }
};

int main() {
  std::cout << "Hello, World" << std::endl;
  return 0;
}
