#include <chip8.hpp>
#include <display.hpp>
#include <chrono> // timing
#include <thread> // sleeping
#include <iostream>

int main() {
  // Window Size: 960x480
  // Internal Resolution: 64x32
  std::cout << "Creating Window..." << std::endl;
  Display display("Chip8 Emulator", 960, 480, 64, 32);
  std::cout << "Window Initialized" << std::endl;
  Chip8 chip8;
  chip8.CPUReset();

  uint32_t videoBuffer[64 * 32]; // because SDL needs 1D array

  // Pitch => Width of one row
  int videoPitch = sizeof(videoBuffer[0]) * 64;

  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  bool quit = false;
  while(!quit){
    quit = display.ProcessInput(chip8.keypad);
    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

    // 60Hz = 16.67ms
    if(dt > 16.67f){
      lastCycleTime = currentTime;

      // Few instructions every frame
      for(int i=0; i<25; i++){
        chip8.cycle();
      }

      chip8.updateTimers();

      // chip8.screenData[y][x] -> videoBuffer
      for(int y = 0; y < 32; y++){
        for(int x = 0; x < 64; x++){
          // Convert 2d to 1d
          int index = (y * 64) + x;

          uint32_t color = (chip8.screenData[y][x] > 0) ? 0xFFFFFFFF : 0x00000000;

          videoBuffer[index] = color;
        }
      }

      display.Update(videoBuffer, videoPitch);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return 0;
}
