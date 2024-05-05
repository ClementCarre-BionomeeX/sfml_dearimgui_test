// Button.h
#pragma once

#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>

class Button : public IWidget {
  public:
    Button(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, int rad);

    void render(SDL_Renderer* renderer) override;
    void update() override;

    Signal<> onClick;
    void     click();

    inline void changeToBaseColor() noexcept { color = &baseColor; }
    inline void changeToHoverColor() noexcept { color = &hoverColor; }

  protected:
    ~Button() {}

  private:
    SDL_Color* color;         // Button color
    SDL_Color  baseColor;     // Button color when not hovered
    SDL_Color  hoverColor;    // Button color when hovered
    int        radius;        // Radius for rounded corners
    bool       clicking = false;
};
