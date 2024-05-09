#pragma once

#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/non_owning_ptr.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Link : public IWidget {
  public:
    Link(non_owning_ptr<IWidget>   source,
         non_owning_ptr<IWidget>   target,
         std::shared_ptr<Relation> relation,
         //  SDL_Color color,
         //  SDL_Color hoverColor,
         int thickness);

    void      render(non_owning_ptr<SDL_Renderer> renderer) override;
    bool      handleEvent(SDL_Event& event) override;
    void      update() override;
    SDL_Point anchor() const noexcept override;

    bool isExtremity(non_owning_ptr<IWidget> w) const noexcept;

    bool isSource(non_owning_ptr<IWidget> w) const noexcept;
    bool isTarget(non_owning_ptr<IWidget> w) const noexcept;
    bool isRelation(std::shared_ptr<Relation> r) const noexcept;

    // debug
    Signal<std::string> debug;

  private:
    non_owning_ptr<IWidget> _source;
    non_owning_ptr<IWidget> _target;

    std::shared_ptr<Relation> _relation;

    // SDL_Color* _color;
    // SDL_Color  _baseColor;
    // SDL_Color  _hoverColor;

    int  _thickness;
    bool isHovered = false;

    bool   isNear(int x, int y) const noexcept;
    double pointLineSegmentDistanceSquared(int x, int y) const noexcept;
    void   draw_indicator(non_owning_ptr<SDL_Renderer> renderer,
                          SDL_Color const&             color) const noexcept;

    // anchor cache
    SDL_Point a{0, 0}, b{0, 0};
};
