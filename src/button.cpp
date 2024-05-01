// Button.cpp
#include "../incl/button.h"
#include "../incl/draw_utils.h"    // Include this if `fillCircle` is defined here

// MARK: Button
Button::Button(int x, int y, int w, int h, SDL_Color col, SDL_Color hoverCol, int rad)
    : IWidget{x, y, w, h}, color(col), hoverColor(hoverCol), isHovered(false), radius(rad) {}

// MARK: render
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

// MARK: handleEvent
bool Button::handleEvent(SDL_Event& event) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    isHovered  = (mouseX >= rect.x) && (mouseX <= rect.x + rect.w) && (mouseY >= rect.y) &&
                (mouseY <= rect.y + rect.h);
    if (event.type == SDL_MOUSEBUTTONUP) {
        if (isHovered && event.button.button == SDL_BUTTON_LEFT) {
            click();
            return true;
        }
    }
    if (isHovered) {
        return true;
    }
    return false;    // Return false if the button was not clicked
}

void Button::update() {}

void Button::click() {
    onClick.emit();
}

std::pair<int, int> Button::anchor() const noexcept {
    return {rect.x + rect.w / 2, rect.y + rect.h / 2};
}
