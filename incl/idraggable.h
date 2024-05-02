#pragma once
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <utility>

class IDraggable : public IWidget {
  public:
    IDraggable() : IWidget{}, baseColor{0, 200, 200, 255}, hoverColor{0, 255, 255, 255} {
        color = baseColor;
    }
    IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor)
        : IWidget{x, y, w, h}, baseColor{baseColor}, hoverColor{hoverColor} {
        color = baseColor;
    }
    virtual ~IDraggable() = default;

    // dragging signals
    Signal<>         onHover;
    Signal<>         onHoverLost;
    Signal<int, int> onMouseLeftDown;
    Signal<int, int> onMouseLeftUp;
    Signal<int, int> onMouseRightDown;
    Signal<int, int> onMouseRightUp;
    Signal<int, int> onDragging;

    inline void changeToBaseColor() noexcept { color = baseColor; }
    inline void changeToHoverColor() noexcept { color = hoverColor; }

    virtual bool handleEvent(SDL_Event& event) override {
        bool handled = false;
        int  mouseX  = event.motion.x;
        int  mouseY  = event.motion.y;
        // Handle hover state
        if (event.type == SDL_MOUSEMOTION) {
            // Update the hover state
            bool currentlyHovering = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                                      mouseY >= rect.y && mouseY <= rect.y + rect.h);
            if (currentlyHovering && !isHover) {
                onHover.emit();
                isHover = true;
            } else if (!currentlyHovering && isHover) {
                onHoverLost.emit();
                isHover = false;
            }

            // Handle dragging
            if (isSelected) {
                if (!isDragging) {
                    isDragging = true;    // Start dragging
                }
                onDragging.emit(mouseX, mouseY);
                handled = true;
            }
        }

        // Handle mouse button events
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT && isHover) {
                isSelected = true;
                onMouseLeftDown.emit(mouseX, mouseY);
                handled = true;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT && isSelected) {
                if (isDragging) {
                    isDragging = false;    // Stop dragging
                }
                onMouseLeftUp.emit(mouseX, mouseY);
                isSelected = false;
                handled    = true;
            }
        }

        return handled;
    }

  protected:
    SDL_Color color;
    SDL_Color baseColor;
    SDL_Color hoverColor;

    bool isSelected = false;
    bool isHover    = false;
    bool isDragging = false;
};
