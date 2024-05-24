#pragma once
#include "../incl/non_owning_ptr.h"
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

    virtual ~IWidget() {
        onHover.disconnect_all();
        onHoverLost.disconnect_all();
        onMouseLeftDown.disconnect_all();
        onMouseLeftUp.disconnect_all();
        onMouseRightDown.disconnect_all();
        onMouseRightUp.disconnect_all();
    };

    virtual bool handleEvent(SDL_Event& event, float zoomfactor) {
        bool handled = false;
        int  mouseX  = (int)((float)(event.motion.x) / zoomfactor);
        int  mouseY  = (int)((float)(event.motion.y) / zoomfactor);

        // Handle hover state
        bool currentlyHovering = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                                  mouseY >= rect.y && mouseY <= rect.y + rect.h);

        if (currentlyHovering && !isHover) {
            onHover.emit();
            isHover = true;
            handled = true;
        } else if (!currentlyHovering && isHover) {
            onHoverLost.emit();
            isHover = false;
        }

        // Handle mouse button events
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT && isHover) {
                onMouseLeftDown.emit(mouseX, mouseY);
                onSelected.emit();
                handled = true;
            }
            if (event.button.button == SDL_BUTTON_LEFT && !isHover) {
                onUnselected.emit();
            }

        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT && isHover) {
                onMouseLeftUp.emit(mouseX, mouseY);
                handled = true;
            }
        }
        // Handle mouse button events
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_RIGHT && isHover) {
                onMouseRightDown.emit(mouseX, mouseY);
                handled = true;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_RIGHT && isHover) {
                onMouseRightUp.emit(mouseX, mouseY);
                handled = true;
            }
        }

        return handled;
    }

    virtual void update()                                                        = 0;
    virtual void render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) = 0;

    virtual SDL_Point anchor() const noexcept { return {rect.x + rect.w / 2, rect.y + rect.h / 2}; }

    virtual void moveTo(int x, int y) noexcept {
        rect.x = x;
        rect.y = y;
    }

    virtual void push(int x, int y) noexcept {
        rect.x += x;
        rect.y += y;
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

    void select() { isSelected = true; }
    void unselect() { isSelected = false; }

    bool is_selected() const { return isSelected; }

    Signal<> onSelected;
    Signal<> onUnselected;

    Signal<>         onHover;
    Signal<>         onHoverLost;
    Signal<int, int> onMouseLeftDown;
    Signal<int, int> onMouseLeftUp;
    Signal<int, int> onMouseRightDown;
    Signal<int, int> onMouseRightUp;

    void mouseLeftDown(int x, int y) { onMouseLeftDown.emit(x, y); }
    void mouseLeftUp(int x, int y) { onMouseLeftUp.emit(x, y); }
    void mouseRightDown(int x, int y) { onMouseRightDown.emit(x, y); }
    void mouseRightUp(int x, int y) { onMouseRightUp.emit(x, y); }

  protected:
    SDL_Rect rect{0, 0, 0, 0};
    bool     isHover    = false;
    bool     isSelected = false;
};
