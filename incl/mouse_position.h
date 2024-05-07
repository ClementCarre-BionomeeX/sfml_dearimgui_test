#include "../incl/iwidget.h"
#include <SDL2/SDL.h>

class MousePosition : public IWidget {
  public:
    MousePosition() {}

    SDL_Point anchor() const noexcept override;
    void      update() override {}
    void      render(SDL_Renderer*) override {}
    SDL_Rect  getRect() const noexcept override;

  private:
};
