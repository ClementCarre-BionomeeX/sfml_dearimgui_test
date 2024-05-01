#pragma once
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <utility>

class Draggable : public IWidget {
  public:
    Draggable(SDL_Color color)
        : IWidget(400, 400, 100, 100), color{color}, isSelected{false}, isHover{false},
          isDragging{false} {}

    bool                handleEvent(SDL_Event& event);
    void                update();
    void                render(SDL_Renderer* renderer);
    std::pair<int, int> anchor() const noexcept;

    SDL_Color        color;
    Signal<>         onHover;
    Signal<>         onHoverLost;
    Signal<int, int> onMouseLeftDown;
    Signal<int, int> onMouseLeftUp;
    Signal<int, int> onMouseRightDown;
    Signal<int, int> onMouseRightUp;
    Signal<int, int> onDragging;

  private:
    bool isSelected;
    bool isHover;
    bool isDragging;
};
