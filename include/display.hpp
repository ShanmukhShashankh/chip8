#pragma once

#include <SDL2/SDL.h>

class Display {
  public:

    // Window Creator
    Display(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

    // Window Destroyer
    ~Display();

    // buffer: Pointer to array of colors
    // pitch: width of one row of pixels in bytes
    void Update(void const* buffer, int pitch);

    // keys: pointer to Chip8's keypad array
    bool ProcessInput(uint8_t* keys);

  private:
    SDL_Window* window; // Frame
    SDL_Renderer* renderer; // Painter
    SDL_Texture* texture; // Canvas
};
