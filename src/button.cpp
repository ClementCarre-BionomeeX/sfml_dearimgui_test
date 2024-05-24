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
void Button::render(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {
    SDL_SetRenderDrawColor(renderer.get(), _color->r, _color->g, _color->b, _color->a);

    // Scale the button's position and size by the zoomFactor
    SDL_Rect zoomedRect = {
        //
        static_cast<int>(static_cast<float>(rect.x) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.y) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.w) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.h) * zoomFactor)     //
    };
    int zoomedRadius = static_cast<int>(static_cast<float>(radius) * zoomFactor);

    // Middle part (adjust the rect to not overwrite the corners)
    SDL_Rect middleRect = {
        zoomedRect.x + zoomedRadius, zoomedRect.y, zoomedRect.w - 2 * zoomedRadius, zoomedRect.h};
    SDL_RenderFillRect(renderer.get(), &middleRect);

    // Top and bottom strips (to fill the gaps in the rounded corners)
    SDL_Rect topRect = {
        zoomedRect.x, zoomedRect.y + zoomedRadius, zoomedRect.w, zoomedRect.h - 2 * zoomedRadius};
    SDL_RenderFillRect(renderer.get(), &topRect);

    // Four corners: top-left, top-right, bottom-left, bottom-right
    fillCircle(renderer, zoomedRect.x + zoomedRadius, zoomedRect.y + zoomedRadius, zoomedRadius);
    fillCircle(renderer,
               zoomedRect.x + zoomedRect.w - zoomedRadius - 1,
               zoomedRect.y + zoomedRadius,
               zoomedRadius);
    fillCircle(renderer,
               zoomedRect.x + zoomedRadius,
               zoomedRect.y + zoomedRect.h - zoomedRadius - 1,
               zoomedRadius);
    fillCircle(renderer,
               zoomedRect.x + zoomedRect.w - zoomedRadius - 1,
               zoomedRect.y + zoomedRect.h - zoomedRadius - 1,
               zoomedRadius);
}

void Button::update() {}

void Button::click() {
    onClick.emit();
}

inline void Button::changeToBaseColor() noexcept {
    _color = std::make_unique<SDL_Color>(_baseColor);
}
inline void Button::changeToHoverColor() noexcept {
    _color = std::make_unique<SDL_Color>(_hoverColor);
}

Button::~Button() {
    onClick.disconnect_all();
}
