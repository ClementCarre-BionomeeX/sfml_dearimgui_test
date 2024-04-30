#pragma once
#include <SDL2/SDL.h>
#include <utility>

class IWidget {
  public:
    virtual ~IWidget()                                         = default;
    virtual bool                handleEvent(SDL_Event& event)  = 0;
    virtual void                update()                       = 0;
    virtual void                render(SDL_Renderer* renderer) = 0;
    virtual std::pair<int, int> anchor() const noexcept        = 0;
};
