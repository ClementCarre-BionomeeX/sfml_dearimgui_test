#pragma once
#include "../incl/button.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextButton : public Button {
  public:
    TextButton(int         x,
               int         y,
               int         w,
               int         h,
               SDL_Color   baseColor,
               SDL_Color   hoverColor,
               int         rad,
               std::string text,
               TTF_Font*   font)
        : Button(x, y, w, h, baseColor, hoverColor, rad), text(text), font(font) {}

    void render(SDL_Renderer* renderer) override;

  private:
    std::string text;
    TTF_Font*   font;
};
