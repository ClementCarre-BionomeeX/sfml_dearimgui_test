#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Node {
  public:
    Node(int x, int y, int w, int h);
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& event);
    void update();

  private:
    SDL_Rect rect;
    int      radius;
};
