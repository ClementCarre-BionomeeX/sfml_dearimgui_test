#include "../incl/draw_utils.h"
#include <cmath>    // Include for sqrt and atan2

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

void roundCornerRectangle(SDL_Renderer* renderer, SDL_Rect rect, int radius) {
    SDL_Rect middleRect = {rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
    SDL_RenderFillRect(renderer, &middleRect);
    // Top and bottom strips (to fill the gaps in the rounded corners)
    SDL_Rect topRect = {rect.x, rect.y + radius, rect.w, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &topRect);
    // Four corners: top-left, top-right, bottom-left, bottom-right
    fillCircle(renderer, rect.x + radius, rect.y + radius, radius);
    fillCircle(renderer, rect.x + rect.w - radius - 1, rect.y + radius, radius);
    fillCircle(renderer, rect.x + radius, rect.y + rect.h - radius - 1, radius);
    fillCircle(renderer, rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius);
}

void drawThickLine(SDL_Renderer* renderer,
                   int           x1,
                   int           y1,
                   int           x2,
                   int           y2,
                   int           thickness,
                   SDL_Color     color) {
    // Bresenham's line algorithm for drawing the line
    int dx  = abs(x2 - x1);
    int dy  = abs(y2 - y1);
    int sx  = x1 < x2 ? 1 : -1;
    int sy  = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SDL_Rect rect = {x1 - thickness / 2, y1 - thickness / 2, thickness, thickness};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);

        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
