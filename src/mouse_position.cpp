#include "../incl/mouse_position.h"

SDL_Point MousePosition::anchor() const noexcept {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

SDL_Rect MousePosition::getRect() const noexcept {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y, 0, 0};
}