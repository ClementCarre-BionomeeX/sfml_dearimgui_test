#pragma once

#include "../incl/button.h"
#include "../incl/idraggable.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include "../incl/textbutton.h"
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

    Signal<> onGlobalMouseLeftUp;
    void     globalMouseLeftUp();
    Signal<> onConnectMouseLeftDown;
    void     connectMouseLeftDown();

  private:
    int radius;
    int margin        = 5;
    int topButtonSize = 20;

    TextButton topButton;
    TextBox    nameTextBox;
    TextButton addConnectionButton;
};
