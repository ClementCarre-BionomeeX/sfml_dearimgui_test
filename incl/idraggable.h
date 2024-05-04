#pragma once
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <utility>

class IDraggable : public IWidget {
  public:
    IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor)
        : IWidget{x, y, w, h}, baseColor{baseColor}, hoverColor{hoverColor} {
        changeToBaseColor();
        onMouseLeftDown.connect([this](int, int) {
            isSelected = true;    // Begin the dragging process
        });

        onMouseLeftUp.connect([this](int, int) {
            if (isDragging) {
                isDragging = false;    // End the dragging process
            }
            isSelected = false;    // Deselect the widget
        });

        onHover.connect([this]() {
            changeToHoverColor();    // Change color on hover
        });

        onHoverLost.connect([this]() {
            changeToBaseColor();    // Revert color when not hovering
        });
    }

    virtual ~IDraggable() = default;

    // dragging signals
    Signal<int, int> onDragging;

    inline void changeToBaseColor() noexcept { color = &baseColor; }
    inline void changeToHoverColor() noexcept { color = &hoverColor; }

    bool handleEvent(SDL_Event& event) override {
        bool handled = IWidget::handleEvent(event);

        // Additional handling for dragging if selected
        if (event.type == SDL_MOUSEMOTION && isSelected) {
            if (!isDragging) {
                isDragging = true;    // Start dragging
            }
            onDragging.emit(event.motion.x, event.motion.y);    // Emit dragging signal
            handled = true;
        }

        return handled;
    }

    virtual std::map<std::string, std::any> serialize() const noexcept {
        auto result = IWidget::serialize();
        result["baseColor"] =
            std::array<int, 4>{baseColor.r, baseColor.g, baseColor.b, baseColor.a};
        result["hoverColor"] =
            std::array<int, 4>{hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a};
        return result;
    }

  protected:
    SDL_Color* color;
    SDL_Color  baseColor;
    SDL_Color  hoverColor;

    bool isSelected = false;
    bool isDragging = false;
};
