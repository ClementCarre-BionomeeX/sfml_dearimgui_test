#include "../incl/iwidget.h"
#include <SDL2/SDL.h>

class MousePosition : public IWidget {
  public:
    MousePosition() {}

    SDL_Point anchor() const noexcept override;
    void      update() override {}
    void      update(float zoomfactor) { _zoomfactor = zoomfactor; }
    void      render(non_owning_ptr<SDL_Renderer>) override {}
    SDL_Rect  getRect() const noexcept override;

  private:
    float _zoomfactor{1.0};
};
