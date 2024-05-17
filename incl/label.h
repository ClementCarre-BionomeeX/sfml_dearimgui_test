#pragma once

#include "../incl/iwidget.h"
#include "../incl/non_owning_ptr.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Label : public IWidget {
  public:
    Label(int                      x,
          int                      y,
          int                      w,
          int                      h,
          std::string const&       _text,
          SDL_Color                _color,
          non_owning_ptr<TTF_Font> _font);
    ~Label();

    void         setText(const std::string& text);
    std::string  getText() const;
    void         setColor(SDL_Color color);
    void         render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    virtual void update() override {}

  private:
    std::string              text;
    SDL_Color                color;
    non_owning_ptr<TTF_Font> font;
};
