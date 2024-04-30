#pragma once

#include "../incl/iwidget.h"
#include "../incl/node.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Link : public IWidget {
  public:
    Link(IWidget* source, IWidget* target, SDL_Color color, SDL_Color hoverColor, int thickness);

    void                render(SDL_Renderer* renderer);
    bool                handleEvent(SDL_Event& event);
    void                update();
    std::pair<int, int> anchor() const noexcept;

  private:
    IWidget*  source;
    IWidget*  target;
    SDL_Color color;
    SDL_Color hoverColor;
    int       thickness;
    bool      isHovered;

    bool   isNear(int x, int y);
    double pointLineDistance(int x, int y);
    double pointLineSegmentDistance(int x, int y);
};
