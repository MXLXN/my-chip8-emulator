#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

#define ORIG_WIDTH      64
#define ORIG_HEIGHT     32

#define OPAQUE          255

#define R_WHITE         255
#define G_WHITE         255
#define B_WHITE         255

#define R_BLACK         0
#define G_BLACK         0
#define B_BLACK         0

extern SDL_Renderer* renderer;
extern SDL_Window* window;

void create_window(void);
void draw_pixel(void);
void close_window(void);

#endif