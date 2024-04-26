// Button.h
#pragma once

#include <SDL2/SDL.h>

struct Button {
    SDL_Rect  rect;          // Position and size of the button
    SDL_Color color;         // Button color
    SDL_Color hoverColor;    // Button color when hovered
    bool      isHovered;     // Is the mouse currently hovering over the button
    int       radius;        // Radius for rounded corners

    Button(int x, int y, int w, int h, SDL_Color col, SDL_Color hoverCol, int rad);

    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& e);
};
