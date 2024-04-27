#include "../incl/draw_utils.h"

// MARK: fillCircle
void fillCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;    // horizontal offset
            int dy = radius - h;    // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}
