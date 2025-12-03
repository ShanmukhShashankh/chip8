#include <chip8.hpp>
#include <chrono> // timing
#include <display.hpp>
#include <iostream>
#include <thread> // sleeping

const int SAMPLE_RATE = 44100;
const int VOLUME = 28;     // (128 +/- 28)
const int FREQUENCY = 440; // A4

// Beep Generator
void AudioCallback(void *userdata, Uint8 *stream, int len) {
  static double runningTime = 0.0;

  for (int i = 0; i < len; i++) {
    double time = runningTime + (double)i / SAMPLE_RATE;
    // Square wave
    double wave = sin(2.0 * M_PI * FREQUENCY * time);

    // 128 -> silent
    // wave > 0 -> 156
    // wave > 0 -> 100
    stream[i] = (wave > 0.0) ? (128 + VOLUME) : (128 - VOLUME);
  }

  runningTime += (double)len / SAMPLE_RATE;
}

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cerr << "Usage: ./<chip8> <path_to_rom>" << std::endl;
    return 1;
  }

  // ====o AUDIO INIT o====

  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Init Failed" << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_AudioSpec want, have;
  SDL_zero(want);
  want.freq = SAMPLE_RATE;
  want.format = AUDIO_U8;
  want.channels = 1;
  want.samples = 2048;
  want.callback = AudioCallback;

  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

  if (dev == 0) {
    std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
  }

  // ====c AUDIO INIT c====

  // Window Size: 960x480
  // Internal Resolution: 64x32
  std::cout << "Creating Window..." << std::endl;
  Display display("Chip8 Emulator", 960, 480, 64, 32);
  std::cout << "Window Initialized" << std::endl;
  Chip8 chip8;
  chip8.CPUReset();

  if (!chip8.loadROM(argv[1])) {
    return 1;
  }

  uint32_t videoBuffer[64 * 32]; // because SDL needs 1D array

  // Pitch => Width of one row
  int videoPitch = sizeof(videoBuffer[0]) * 64;

  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  bool quit = false;
  while (!quit) {
    quit = display.ProcessInput(chip8.keypad);
    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
                   currentTime - lastCycleTime)
                   .count();

    // 60Hz = 16.67ms
    if (dt > 16.67f) {
      lastCycleTime = currentTime;

      // Few instructions every frame
      for (int i = 0; i < 25; i++) {
        chip8.cycle();
      }

      chip8.updateTimers();

      if (chip8.isSoundActive()) {
        SDL_PauseAudioDevice(dev, 0);
      } else {
        SDL_PauseAudioDevice(dev, 1);
      }

      // chip8.screenData[y][x] -> videoBuffer
      for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
          // Convert 2d to 1d
          int index = (y * 64) + x;

          uint32_t color =
              (chip8.screenData[y][x] > 0) ? 0xFFFFFFFF : 0x00000000;

          videoBuffer[index] = color;
        }
      }

      display.Update(videoBuffer, videoPitch);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  SDL_CloseAudioDevice(dev);
  return 0;
}
