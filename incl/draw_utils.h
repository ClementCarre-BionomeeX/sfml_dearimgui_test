#pragma once

#include <SDL2/SDL.h>

// Declare the function
void fillCircle(SDL_Renderer* renderer, int cx, int cy, int radius);

void roundCornerRectangle(SDL_Renderer* renderer, SDL_Rect rect, int radius);

void drawThickLine(SDL_Renderer*    renderer,
                   int              x1,
                   int              y1,
                   int              x2,
                   int              y2,
                   int              thickness,
                   SDL_Color const& color);
