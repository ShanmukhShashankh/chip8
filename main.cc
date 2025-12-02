#include <cstdint>
#include <display.hpp>
#include <iostream>

int main() {
  // Window Size: 960x480
  // Internal Resolution: 64x32
  std::cout << "Creating Window..." << std::endl;
  Display display("Chip8 Testing", 960, 480, 64, 32);
  std::cout << "Window Initialized" << std::endl;

  uint8_t dummyKeypad[16] = {0};

  bool quit = false;
  while(!quit){
    quit = display.ProcessInput(dummyKeypad);
    SDL_Delay(16);
  }

  std::cout << "Closing Window" << std::endl;

  return 0;
}
