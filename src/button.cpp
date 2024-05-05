// Button.cpp
#include "../incl/button.h"
#include "../incl/draw_utils.h"    // Include this if `fillCircle` is defined here

// MARK: Button
Button::Button(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, int rad)
    : IWidget{x, y, w, h}, _baseColor(baseColor), _hoverColor(hoverColor), radius(rad) {
    changeToBaseColor();
    onHover.connect([this]() {
        changeToHoverColor();    // Change color on hover
    });

    onHoverLost.connect([this]() {
        changeToBaseColor();    // Revert color when not hovering
        clicking = false;
    });

    onMouseLeftDown.connect([this](int, int) { clicking = true; });
    onMouseLeftUp.connect([this](int, int) {
        if (clicking) {
            click();
        }
        clicking = false;
    });
}

// MARK: render
void Button::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, _color->r, _color->g, _color->b, _color->a);

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

void Button::update() {}

void Button::click() {
    onClick.emit();
}
