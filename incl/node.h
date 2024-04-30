#pragma once

#include "../incl/button.h"
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <utility>

class Node : public IWidget {
  public:
    Node(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor);
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& event);
    void update();

    std::pair<int, int> anchor() const noexcept;
    Signal<>            onTopButtonClick;

  private:
    SDL_Rect  rect;
    int       radius;
    bool      isHovered;
    SDL_Color color;
    SDL_Color hoverColor;
    bool      isSelected;
    int       startmoveX;
    int       startmoveY;
    Button    button;
};
