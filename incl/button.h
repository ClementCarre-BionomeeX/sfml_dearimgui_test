// Button.h
#pragma once

#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>

class Button : public IWidget {
  public:
    Button(int x, int y, int w, int h, SDL_Color col, SDL_Color hoverCol, int rad);
    void                render(SDL_Renderer* renderer);
    bool                handleEvent(SDL_Event& event);
    void                update();
    std::pair<int, int> anchor() const noexcept;

    Signal<> onClick;

    void click();

    void move(int x, int y) noexcept;
    void resize(int w, int h) noexcept;

  private:
    SDL_Rect  rect;          // Position and size of the button
    SDL_Color color;         // Button color
    SDL_Color hoverColor;    // Button color when hovered
    bool      isHovered;     // Is the mouse currently hovering over the button
    int       radius;        // Radius for rounded corners
};
