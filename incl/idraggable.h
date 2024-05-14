#pragma once
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <utility>

class IDraggable : public IWidget {
  public:
    IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor);

    virtual ~IDraggable() { onDragging.disconnect_all(); };

    // dragging signals
    Signal<int, int> onDragging;
    void             drag(int x, int y);

    void changeToBaseColor() noexcept;
    void changeToHoverColor() noexcept;

    bool handleEvent(SDL_Event& event, float zoomfactor) override;

    virtual std::map<std::string, std::any> serialize() const noexcept {
        auto result = IWidget::serialize();
        result["baseColor"] =
            std::array<int, 4>{_baseColor.r, _baseColor.g, _baseColor.b, _baseColor.a};
        result["hoverColor"] =
            std::array<int, 4>{_hoverColor.r, _hoverColor.g, _hoverColor.b, _hoverColor.a};
        return result;
    }

  protected:
    std::unique_ptr<SDL_Color> _color;
    SDL_Color                  _baseColor;
    SDL_Color                  _hoverColor;

    bool isSelected = false;
    bool isDragging = false;
};
