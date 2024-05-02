#pragma once

#include "../incl/button.h"
// #include "../incl/iwidget.h"
#include "../incl/idraggable.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <utility>

class Node : public IDraggable {
  public:
    Node(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, TTF_Font* font);

    void render(SDL_Renderer* renderer) override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;

    Signal<> onTopButtonClick;
    void     topButtonClick();

    Signal<std::string> onNameChanged;
    void                changeName(std::string name);

  private:
    int radius;

    Button  button;
    TextBox nameTextBox;
};
