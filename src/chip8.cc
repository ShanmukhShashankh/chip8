#include <chip8.hpp>
#include <cstdio>
#include <cstring>
#include <iostream>

const BYTE Chip8::fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() { Chip8::CPUReset(); }

void Chip8::CPUReset() {
  addressI = 0;
  programCounter = 0x200; // Valid game address starts from 0x200
  stackPointer = -1;      // Reset Stack Pointer
  memset(registers, 0, sizeof(registers));
  memset(gameMemory, 0, sizeof(gameMemory));
  memset(screenData, 0, sizeof(screenData));
  memset(keypad, 0, sizeof(keypad));

  FILE *in;
  in = fopen("/mnt/BambooCopter/Games/INVADERS", "rb");
  if (in) {
    fread(&gameMemory[0x200], 0xE00, 1, in);
    fclose(in);
  } else {
    std::cerr << "Failed to load ROM" << std::endl;
  }

  // Font Data
  memcpy(&gameMemory[0x50], &fontset, sizeof(fontset));
}

void Chip8::cycle() {
  WORD opcode = gameMemory[programCounter]; // UB
  opcode <<= 8;                             // LB to UB
  opcode |= gameMemory[programCounter + 1]; // slot in the LB
  programCounter += 2;
  decode(opcode);
}

void Chip8::updateTimers() {
  if (delayTimer > 0)
    delayTimer--;
  if (soundTimer > 0)
    soundTimer--;
}

void Chip8::decode(WORD opcode) {
    switch (opcode >> 12) {

    case 0x0:
      switch (opcode & 0xFF) {
      case 0xE0: // 00E0 | CLS display
        memset(screenData, 0, sizeof(screenData));
        break;

      case 0xEE: // 00EE | RET from subroutine
        programCounter = stack[stackPointer];
        stackPointer--;
        break;
      }
      break;

    case 0x1: // 1nnn | JP to 1nnn
      programCounter = opcode & 0xFFF;
      break;

    case 0x2: // 2nnn | CALL addr nnn
      stackPointer++;
      stack[stackPointer] = programCounter;
      programCounter = opcode & 0xFFF;

    case 0x3: // 3xkk | SE (skip) if Vx = kk
    {
      BYTE reg = opcode >> 8 & 0xF; // x
      if (registers[reg] == (opcode & 0xFF)) {
        programCounter += 2;
      }
    } break;

    case 0x4: // 4xkk | SNE (skip) if Vx != kk
    {
      BYTE reg = opcode >> 8 & 0xF; // x
      if (registers[reg] != (opcode & 0xFF)) {
        programCounter += 2;
      }
    } break;

    case 0x5: // 5xy0 | SE (skip) if Vx = Vy
    {
      BYTE Vx = opcode >> 8 & 0xF;
      BYTE Vy = opcode >> 4 & 0xF;

      if (registers[Vx] == registers[Vy]) {
        programCounter += 2;
      }
    } break;

    case 0x6: // 6xkk | LD kk into Vx register
    {
      BYTE reg = opcode >> 8 & 0xF; // x
      registers[reg] = opcode & 0xFF;
    } break;

    case 0x7: // 7xkk | ADD Vx, kk
    {
      BYTE reg = opcode >> 8 & 0xF; // x
      registers[reg] += opcode & 0xFF;
    } break;

    case 0x8: // ALU instructions
    {
      BYTE Vx = opcode >> 8 & 0xF;
      BYTE Vy = opcode >> 4 & 0xF;

      switch (opcode & 0xF) {
      case 0x0: // 8xy0 | LD Vx, Vy
        registers[Vx] = registers[Vy];
        break;

      case 0x1: // 8xy1 | OR Vx, Vy
        registers[Vx] |= registers[Vy];
        break;

      case 0x2: // 8xy2 | AND Vx, Vy
        registers[Vx] &= registers[Vy];
        break;

      case 0x3: // 8xy3 | XOR Vx, Vy
        registers[Vx] ^= registers[Vy];
        break;

      case 0x4: // 8xy4 | ADD Vx, Vy
      {
        WORD sum = registers[Vx] + registers[Vy];
        if (sum > 255) {
          registers[0xF] = 0x1;
        } else {
          registers[0xF] = 0x0;
        }
        registers[Vx] = sum & 0xFF;
      } break;

      case 0x5: // 8xy5 | SUB Vx, Vy
        if (registers[Vx] > registers[Vy]) {
          registers[0xF] = 1; // No Borrow
        } else {
          registers[0xF] = 0; // Borrow
        }
        registers[Vx] = registers[Vx] - registers[Vy];
        break;

      case 0x6: // 8xy6 | SHR Vx
        if ((registers[Vx] & 0x1) == 0x1) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[Vx] >>= 1;
        break;

      case 0x7: // 8xy7 | SUBN Vx, Vy
        if (registers[Vx] < registers[Vy]) {
          registers[0xF] = 1; // Borrow
        } else {
          registers[0xF] = 0; // Not Borrow
        }
        registers[Vx] = registers[Vy] - registers[Vx];
        break;

      case 0xE: // 8xyE | SHL vx
        if ((registers[Vx] >> 7 & 0x1) == 0x1) {
          registers[0xF] = 1;
        } else {
          registers[0xF] = 0;
        }
        registers[Vx] <<= 1;
        break;
      }
    } break;

    case 0x9: // 9xy0 | SNE (skip) if Vx != Vy
    {
      BYTE Vx = opcode >> 8 & 0xF;
      BYTE Vy = opcode >> 4 & 0xF;

      if (registers[Vx] != registers[Vy]) {
        programCounter += 2;
      }
    } break;

    case 0xA: // Annn | LD I, addr
      addressI = opcode & 0xFFF;
      break;

    case 0xB: // Bnnn | JP V0, addr : Jump to nnn + V0
      programCounter = (opcode & 0xFFF) + registers[0];
      break;

    case 0xC: // Cxkk | RND Vx, byte
    {
      BYTE Vx = opcode >> 8 & 0xF;
      BYTE randomByte = rand();
      registers[Vx] = opcode & 0xFF & randomByte;
    } break;

    case 0xD: // Dxyn | DRW Vx, Vy, nibble
    {
      BYTE Vx = opcode >> 8 & 0xF;
      BYTE Vy = opcode >> 4 & 0xF;
      BYTE height = opcode & 0xF;
      BYTE x = registers[Vx];
      BYTE y = registers[Vy];

      registers[0xF] = 0; // reset

      for (BYTE i = 0; i < height; i++) {
        BYTE sprite = gameMemory[addressI + i];
        for (BYTE j = 0; j < 8; j++) {
          BYTE spritePixel = sprite & (0x80 >> j);
          BYTE *screenPixel = &screenData[(y + i) % 32][(x + j) % 64];

          if (spritePixel) {
            if (*screenPixel == 0xFF) {
              registers[0xF] = 1;
            }
            *screenPixel ^= 0xFF;
          }
        }
      }
    } break;

    case 0xE:
      switch (opcode & 0xFF) {
      case 0x9E: // Ex9E | SKP Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        if (keypad[registers[Vx]] == 0xFF) {
          programCounter += 2;
        }
      } break;

      case 0xA1: // ExA1 | SKNP Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        if (keypad[registers[Vx]] != 0xFF) {
          programCounter += 2;
        }
      } break;
      }
      break;

    case 0xF:
      switch (opcode & 0xFF) {
      case 0x07: // Fx07 | LD Vx, DT
      {
        BYTE Vx = opcode >> 8 & 0xF;
        registers[Vx] = delayTimer;
      } break;

      case 0x0A: // Fx0A | LD Vx, K
      {
        BYTE Vx = opcode >> 8 & 0xF;
        bool keypressFound = false;
        for (BYTE i = 0; i < 16; i++) {
          if (keypad[i]) {
            registers[Vx] = i;
            keypressFound = true;
            break;
          }
        }
        if (!keypressFound)
          programCounter -= 2;
      } break;

      case 0x15: // Fx15 | LD DT, Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        delayTimer = registers[Vx];
      } break;

      case 0x18: // Fx18 | LD ST, Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        soundTimer = registers[Vx];
      } break;

      case 0x1E: // Fx1E | ADD I, Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        addressI += registers[Vx];
      } break;

      case 0x29: // Fx29 | LD F, Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        BYTE num = registers[Vx];

        addressI = num * 5 + 0x50; // <Offset> + Start_address>
      } break;

      case 0x33: // Fx33 | LD B, Vx : Store BCD of Vx in I, I+1, I+2
      {
        BYTE Vx = opcode >> 8 & 0xF;
        BYTE num = registers[Vx];
        gameMemory[addressI] = num / 100;
        gameMemory[addressI + 1] = (num / 10) % 10;
        gameMemory[addressI + 2] = num % 10;
      } break;

      case 0x55: // Fx55 | LD [I], Vx
      {
        BYTE Vx = opcode >> 8 & 0xF;
        WORD oldAddr = addressI;
        for (unsigned int i = 0; i <= Vx; i++) {
          gameMemory[addressI] = registers[i];
          addressI++;
        }
        addressI = oldAddr;
      } break;

      case 0x65: // Fx65 | LD Vx, [I]
      {
        BYTE Vx = opcode >> 8 & 0xF;
        WORD oldAddr = addressI;
        for (unsigned int i = 0; i <= Vx; i++) {
          registers[i] = gameMemory[addressI];
          addressI++;
        }
        addressI = oldAddr;
      } break;
      }
      break;
    }
}

