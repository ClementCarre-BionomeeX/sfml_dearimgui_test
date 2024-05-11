#pragma once
#include "../incl/button.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextButton : public Button {
  public:
    TextButton(int                      x,
               int                      y,
               int                      w,
               int                      h,
               SDL_Color                baseColor,
               SDL_Color                hoverColor,
               int                      rad,
               std::string              text,
               non_owning_ptr<TTF_Font> font)
        : Button(x, y, w, h, baseColor, hoverColor, rad), _text(text), _font(font) {}

    void render(non_owning_ptr<SDL_Renderer> renderer) override;

  private:
    std::string              _text;
    non_owning_ptr<TTF_Font> _font;
};
