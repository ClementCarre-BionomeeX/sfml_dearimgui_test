#pragma once
#include <SDL2/SDL.h>

class IWidget {
  public:
    virtual ~IWidget()                          = default;
    virtual bool handleEvent(SDL_Event& event)  = 0;
    virtual void update()                       = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};
