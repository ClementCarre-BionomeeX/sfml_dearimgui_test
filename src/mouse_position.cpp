#include "../incl/mouse_position.h"

SDL_Point MousePosition::anchor() const noexcept {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return {(int)((float)x / _zoomfactor), (int)((float)y / _zoomfactor)};
}

SDL_Rect MousePosition::getRect() const noexcept {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return {(int)((float)x / _zoomfactor), (int)((float)y / _zoomfactor), 0, 0};
}
