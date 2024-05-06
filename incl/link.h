#pragma once

#include "../incl/iwidget.h"
#include "../incl/node.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Link : public IWidget {
  public:
    Link(IWidget* source, IWidget* target, SDL_Color color, SDL_Color hoverColor, int thickness);

    void      render(SDL_Renderer* renderer) override;
    bool      handleEvent(SDL_Event& event) override;
    void      update() override;
    SDL_Point anchor() const noexcept override;

    bool isExtremity(IWidget* w) const noexcept;

    bool isSource(IWidget* w) const noexcept;
    bool isTarget(IWidget* w) const noexcept;

  private:
    IWidget*   _source;
    IWidget*   _target;
    SDL_Color* _color;
    SDL_Color  _baseColor;
    SDL_Color  _hoverColor;
    int        _thickness;
    bool       isHovered = false;

    bool   isNear(int x, int y) const noexcept;
    double pointLineSegmentDistanceSquared(int x, int y) const noexcept;
    void   draw_indicator(SDL_Renderer* renderer) const noexcept;

    // anchor cache
    SDL_Point a{0, 0}, b{0, 0};
};
