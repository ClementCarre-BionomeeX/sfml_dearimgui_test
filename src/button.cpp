// Button.cpp
#include "../incl/button.h"
#include "../incl/draw_utils.h"    // Include this if `fillCircle` is defined here

Button::Button(int x, int y, int w, int h, SDL_Color col, SDL_Color hoverCol, int rad)
    : rect{x, y, w, h}, color(col), hoverColor(hoverCol), isHovered(false), radius(rad) {}

void Button::render(SDL_Renderer* renderer) {
    SDL_Color currentColor = isHovered ? hoverColor : color;
    SDL_SetRenderDrawColor(
        renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);

    // Middle part (adjust the rect to not overwrite the corners)
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

bool Button::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) {
        int mouseX = e.motion.x;
        int mouseY = e.motion.y;
        isHovered  = (mouseX >= rect.x) && (mouseX <= rect.x + rect.w) && (mouseY >= rect.y) &&
                    (mouseY <= rect.y + rect.h);
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        if (isHovered && e.button.button == SDL_BUTTON_LEFT) {
            return true;    // Return true if the button was clicked
        }
    }
    return false;    // Return false if the button was not clicked
}
