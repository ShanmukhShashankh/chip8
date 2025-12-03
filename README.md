# Chip-8 Emulator

A functional Chip-8 interpreter written in C++ using SDL2 for graphics and audio. This project emulates the original COSMAC VIP Chip-8 architecture, supporting video output, sound synthesis, and keyboard input.

## Features

* **Core Emulation:** Full implementation of the standard Chip-8 opcode set.
* **Graphics:** 64x32 monochrome display rendered via SDL2 textures (scaled for modern screens).
* **Sound:** 8-bit generated square wave audio.
* **Timing:** 60Hz display refresh rate with a ~600Hz CPU cycle speed (adjustable).
* **Input:** Hex keypad mapped to standard QWERTY keyboard.
* **Architecture:** Clean separation between Core CPU logic (`Chip8` library) and Platform logic (`SDL2` display/input).

## Prerequisites

To build this project, you need the following installed on your system:

* **C++ Compiler** (GCC or Clang) supporting C++17 or later.
* **CMake** (Version 3.10+)
* **SDL2 Development Libraries**

### Installing Dependencies (Arch Linux)
```bash
sudo pacman -S base-devel cmake sdl2
```

### Installing Dependencies (Ubuntu/Debian)
```bash
sudo apt-get install build-essential cmake libsdl2-dev
```

## Build Instructions

### 1. Clone the Repository:
```bash
git clone https://github.com/ShanmukhShashankh/chip8 
cd chip8 
```
### 2. Create a build directory:
```bash
mkdir build
cd build
```

### 3. Configure and Compile:
```bash
cmake ..
make
```

## Usage

Run the emulator by providing the path to a Chip-8 ROM file:
```bash
./chip8 ./path/to/ROM
```

Example:
```
./chip8 ../roms/INVADERS.ch8
```

## Controls

The original Chip-8 used a hex keypad (0-F). This emulator maps them to the left side of a QWERTY keyboard:
| Chip-8 Key | Keyboard Mapping |
| :--------: | :--------------: |
|      1     |         1        |
|      2     |         2        |
|      3     |         3        |
|      C     |         4        |
|      4     |         Q        |
|      5     |         W        |
|      6     |         E        |
|      D     |         R        |
|      7     |         A        |
|      8     |         S        |
|      9     |         D        |
|      E     |         F        |
|      A     |         Z        |
|      0     |         X        |
|      F     |         V        |
|      B     |         C        |

### System Keys:

`ESC` : Quit Emulator

## Project Structure:
```bash
.
├── CMakeLists.txt
├── include
│   ├── chip8.hpp
│   ├── display.hpp
│   └── types.h
├── LICENSE
├── main.cc
├── README.md
└── src
    ├── chip8.cc
    └── display.cc
```

## Notes

This emulator implements the standard Chip-8 behavior (optimized for Space Invaders). Some newer ROMs that rely on the "Shift Quirk" or "Load/Store Quirk" (like BRIX or BLITZ) may exhibit minor visual or logic glitches.

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.

Permissions of this strong copyleft license are conditioned on making available complete source code of licensed works and modifications, which include larger works using a licensed work, under the same license. Copyright and license notices must be preserved.
