#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "frame.h"
#include "render.h"
#include "simd.h"

#define PI 3.14159265359
#define HPI (PI * 0.5)

int main(void) {
  int ratio = 3;
  int w = 1440 / ratio, h = 900 / ratio;

  SDL_Window *win = SDL_CreateWindow("R1", 1440, 900, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(win, NULL);

  SDL_Texture *gpu = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                       SDL_TEXTUREACCESS_STREAMING, w, h);
  SDL_SetTextureScaleMode(gpu, SDL_SCALEMODE_NEAREST);

  RFrame *frame = rohan_create_frame(w, h);
  int *pixels = rohan_obtain_buffer(frame);

  init_simd_tables();

  clock_t start = clock();

  unsigned frames = 0;
  float angle = 0.f;
  for (const bool *keys = SDL_GetKeyboardState(NULL); !keys[SDL_SCANCODE_Q];
       SDL_PumpEvents()) {
    frames += 1;
    angle += 0.0001f;

    float r = 360.f / ratio;
    float x0 = cosf(angle) * r + w / 2, y0 = sinf(angle) * r + h / 2;
    float x1 = cosf(angle + HPI) * r + w / 2,
          y1 = sinf(angle + HPI) * r + h / 2;
    float x2 = cosf(angle + PI) * r + w / 2, y2 = sinf(angle + PI) * r + h / 2;
    float x3 = cosf(angle - HPI) * r + w / 2,
          y3 = sinf(angle - HPI) * r + h / 2;

    // float x0 = 0.f, y0 = 0.f;
    // float x1 = w, y1 = 0.f;
    // float x2 = w, y2 = h;
    // float x3 = 0.f, y3 = h;

    const float N = 16.f;
    const float inv_N = 1.f / N;

    /*        render_triangle(pixels, w, roundf(x0 * N) * inv_N, roundf(y0 * N)
       * inv_N, roundf(x1 * N) * inv_N, roundf(y1 * N) * inv_N, roundf(x2 * N) *
       inv_N, roundf(y2 * N) * inv_N);

            render_triangle(pixels, w, roundf(x0 * N) * inv_N, roundf(y0 * N) *
       inv_N, roundf(x2 * N) * inv_N, roundf(y2 * N) * inv_N, roundf(x3 * N) *
       inv_N, roundf(y3 * N) * inv_N);
    */

    render_line(pixels, w, x0, y0, x1, y1, 0xff0000ff);
    render_line(pixels, w, x1, y1, x2, y2, 0xff00ff00);
    render_line(pixels, w, x2, y2, x0, y0, 0xffff0000);

    /*int pitch = 0;
    void *dest = NULL;
    SDL_LockTexture(gpu, NULL, &dest, &pitch);

    for (int y = 0; y < h; ++y)
      memcpy((char *)dest + y * pitch, pixels + y * w, w * sizeof(int));

    SDL_UnlockTexture(gpu);
    */

    SDL_UpdateTexture(gpu, NULL, pixels, w * sizeof(int));

    SDL_RenderTexture(renderer, gpu, NULL, NULL);
    SDL_RenderPresent(renderer);

    memset(pixels, 0, w * h * sizeof(int));
    SDL_RenderClear(renderer);
  }

  double elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
  printf("average ms/f: %f\n", (float)(elapsed / frames) * 1000.f);

  rohan_free_frame(frame);

  return 0;
}
