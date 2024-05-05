#include "../incl/iwidget.h"
#include <SDL2/SDL.h>

class MousePosition : public IWidget {
  public:
    MousePosition() {}

    std::pair<int, int> anchor() const noexcept override {
        int x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }
    void update() override {}
    void render(SDL_Renderer* renderer) override {}

  private:
};
