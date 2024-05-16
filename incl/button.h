// Button.h
#pragma once

#include "../incl/iwidget.h"
#include "../incl/non_owning_ptr.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <memory>

class Button : public IWidget {
  public:
    Button(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, int rad);

    void render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    void update() override;

    Signal<> onClick;
    void     click();

    inline void changeToBaseColor() noexcept;
    inline void changeToHoverColor() noexcept;

    ~Button();

  private:
    std::unique_ptr<SDL_Color> _color;         // Button color
    SDL_Color                  _baseColor;     // Button color when not hovered
    SDL_Color                  _hoverColor;    // Button color when hovered
    int                        radius;         // Radius for rounded corners
    bool                       clicking = false;
};
