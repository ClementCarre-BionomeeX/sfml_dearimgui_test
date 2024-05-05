#pragma once
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <utility>

class IDraggable : public IWidget {
  public:
    IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor)
        : IWidget{x, y, w, h}, _baseColor{baseColor}, _hoverColor{hoverColor} {
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

    inline void changeToBaseColor() noexcept { _color = &_baseColor; }
    inline void changeToHoverColor() noexcept { _color = &_hoverColor; }

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

    virtual std::map<std::string, std::any> serialize() const noexcept override {
        auto result = IWidget::serialize();
        result["baseColor"] =
            std::array<int, 4>{_baseColor.r, _baseColor.g, _baseColor.b, _baseColor.a};
        result["hoverColor"] =
            std::array<int, 4>{_hoverColor.r, _hoverColor.g, _hoverColor.b, _hoverColor.a};
        return result;
    }

  protected:
    SDL_Color* _color;
    SDL_Color  _baseColor;
    SDL_Color  _hoverColor;

    bool isSelected = false;
    bool isDragging = false;
};
