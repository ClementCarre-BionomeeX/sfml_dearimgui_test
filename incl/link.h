#pragma once

#include "../incl/iwidget.h"
#include "../incl/json.h"
#include "../incl/node.h"
#include "../incl/non_owning_ptr.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
using json = nlohmann::json;

class Link : public IWidget {
  public:
    Link(std::weak_ptr<IWidget>  source,
         std::weak_ptr<IWidget>  target,
         std::weak_ptr<Relation> relation,
         int                     thickness);

    ~Link() { debug.disconnect_all(); }

    void      render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    bool      handleEvent(SDL_Event& event, float zoomfactor) override;
    void      update() override;
    SDL_Point anchor() const noexcept override;

    bool isExtremity(std::weak_ptr<IWidget> w) const noexcept;

    bool isSource(std::weak_ptr<IWidget> w) const noexcept;
    bool isTarget(std::weak_ptr<IWidget> w) const noexcept;
    bool isRelation(std::weak_ptr<Relation> r) const noexcept;

    bool isRelationDirected() const noexcept;

    std::weak_ptr<Relation> getRelation() const noexcept;

    // debug
    Signal<std::string> debug;

    void select() { isSelected = true; }
    void unselect() { isSelected = false; }

    json save(std::vector<std::weak_ptr<IWidget>> const&  widgets,
              std::vector<std::weak_ptr<Relation>> const& relations) const;

    std::weak_ptr<IWidget> getSource() const;
    std::weak_ptr<IWidget> getTarget() const;

  private:
    std::weak_ptr<IWidget> _source;
    std::weak_ptr<IWidget> _target;

    std::weak_ptr<Relation> _relation;

    int  _thickness;
    bool isHovered = false;

    bool   isNear(int x, int y) const noexcept;
    double pointLineSegmentDistanceSquared(int x, int y) const noexcept;
    void   draw_indicator(non_owning_ptr<SDL_Renderer> renderer,
                          SDL_Color const&             color,
                          float                        zoomfactor) const noexcept;

    // anchor cache
    SDL_Point a{0, 0}, b{0, 0};
};
