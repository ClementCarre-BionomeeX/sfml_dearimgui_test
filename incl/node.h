#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Node {
  public:
    Node(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor);
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& event);
    void update();

  private:
    SDL_Rect  rect;
    int       radius;
    bool      isHovered;
    SDL_Color color;
    SDL_Color hoverColor;
};
