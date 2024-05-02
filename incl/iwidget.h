#pragma once
#include <SDL2/SDL.h>
#include <utility>

class IWidget {
  public:
    IWidget() : rect{0, 0, 0, 0} {}
    IWidget(int x, int y, int w, int h) : rect{x, y, w, h} {}
    virtual ~IWidget()                          = default;
    virtual bool handleEvent(SDL_Event& event)  = 0;
    virtual void update()                       = 0;
    virtual void render(SDL_Renderer* renderer) = 0;

    virtual std::pair<int, int> anchor() const noexcept {
        return {rect.x + rect.w / 2, rect.y + rect.h / 2};
    }

    virtual void moveTo(int x, int y) noexcept {
        rect.x = x;
        rect.y = y;
    }

    virtual void resize(int w, int h) noexcept {
        rect.w = w;
        rect.h = h;
    }

    virtual std::pair<int, int> position() const noexcept { return {rect.x, rect.y}; }

    virtual SDL_Rect getRect() const noexcept { return rect; }

  protected:
    SDL_Rect rect;
};
