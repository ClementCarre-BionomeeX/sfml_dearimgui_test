// Button.h
#pragma once

#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>

class Button : public IWidget {
  public:
    Button(int x, int y, int w, int h, SDL_Color col, SDL_Color hoverCol, int rad);
    void                render(SDL_Renderer* renderer) override;
    bool                handleEvent(SDL_Event& event) override;
    void                update() override;
    std::pair<int, int> anchor() const noexcept override;

    Signal<> onClick;

    void click();

  private:
    // SDL_Rect  rect;          // Position and size of the button
    SDL_Color color;         // Button color
    SDL_Color hoverColor;    // Button color when hovered
    bool      isHovered;     // Is the mouse currently hovering over the button
    int       radius;        // Radius for rounded corners
};
