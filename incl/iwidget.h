#pragma once
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <any>
#include <array>
#include <iostream>
#include <map>
#include <string>
#include <utility>

class IWidget {
  public:
    IWidget() : rect{0, 0, 0, 0} {}
    IWidget(int x, int y, int w, int h) : rect{x, y, w, h} {}

    virtual ~IWidget() = default;

    virtual bool handleEvent(SDL_Event& event) {
        bool handled = false;
        int  mouseX  = event.motion.x;
        int  mouseY  = event.motion.y;

        // Handle hover state
        bool currentlyHovering = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                                  mouseY >= rect.y && mouseY <= rect.y + rect.h);
        if (currentlyHovering && !isHover) {
            onHover.emit();
            isHover = true;
        } else if (!currentlyHovering && isHover) {
            onHoverLost.emit();
            isHover = false;
        }

        // Handle mouse button events
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT && isHover) {
                onMouseLeftDown.emit(mouseX, mouseY);
                handled = true;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT && isHover) {
                onMouseLeftUp.emit(mouseX, mouseY);
                handled = true;
            }
        }

        return handled;
    }

    virtual void update()                       = 0;
    virtual void render(SDL_Renderer* renderer) = 0;

    virtual SDL_Point anchor() const noexcept { return {rect.x + rect.w / 2, rect.y + rect.h / 2}; }

    virtual void moveTo(int x, int y) noexcept {
        rect.x = x;
        rect.y = y;
    }

    virtual void resize(int w, int h) noexcept {
        rect.w = w;
        rect.h = h;
    }

    virtual SDL_Point position() const noexcept { return {rect.x, rect.y}; }

    virtual SDL_Rect getRect() const noexcept { return rect; }

    virtual std::map<std::string, std::any> serialize() const noexcept {
        auto result    = std::map<std::string, std::any>{};
        result["rect"] = std::array<int, 4>{rect.x, rect.y, rect.w, rect.h};
        return result;
    }

    Signal<>         onHover;
    Signal<>         onHoverLost;
    Signal<int, int> onMouseLeftDown;
    Signal<int, int> onMouseLeftUp;
    Signal<int, int> onMouseRightDown;
    Signal<int, int> onMouseRightUp;

  protected:
    SDL_Rect rect{0, 0, 0, 0};
    bool     isHover = false;
};
