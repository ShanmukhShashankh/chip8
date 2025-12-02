#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_render.h>
#include <display.hpp>
#include <iostream>

Display::Display(char const *title, int windowWidth, int windowHeight,
                 int textureWidth, int textureHeight) {

  SDL_Init(SDL_INIT_VIDEO);

  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_SHOWN);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, textureWidth,
                              textureHeight);

  SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

Display::~Display() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Display::Update(void const *buffer, int pitch) {
  SDL_UpdateTexture(texture, nullptr, buffer, pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}
bool Display::ProcessInput(uint8_t *keys) {
  bool quit = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      quit = true;
    } break;

    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE)
        quit = true;
    } break;
    }
  }
  return quit;
}
