#include <stdio.h>

#define SDL_MAIN_HANDLED

#ifdef _MSC_VER
#include "SDL/include/SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#include "Platform.h"
#include "metron/pong.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* args[]) {
  printf("Hello SDL-under-WSL World!\n");
  SDL_Window* window = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }
  window = SDL_CreateWindow(
          "hello_sdl2",
          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          SCREEN_WIDTH, SCREEN_HEIGHT,
          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
          );
  if (window == NULL) {
    printf("could not create window: %s\n", SDL_GetError());
    return 1;
  }

  bool quit = false;
  const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

  //----------

  Pong pong;

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) quit = true;
    }

    auto time_a = timestamp();

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_LockSurface(surface);
    Uint32 *buffer = (Uint32*) surface->pixels;

    for (int i = 0; i < 1000000; i++) {
      pong.tock_video();
      pong.tock_game(0,0);
      auto pix_x = pong.tock_pix_x();
      auto pix_y = pong.tock_pix_y();

      if (pix_x < 640 && pix_y < 480) {
        uint8_t r = pong.vga_R * 255;
        uint8_t g = pong.vga_G * 255;
        uint8_t b = pong.vga_B * 255;
        buffer[pix_x + pix_y * SCREEN_WIDTH] = (r << 16) | (g << 8) | (b << 0);
      }
    }

    auto time_b = timestamp();
    printf("%2.2f\n", float(time_b - time_a) / 1000000.0);

    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
  }

  //----------

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
