#pragma once

#include "../incl/button.h"
// #include "../incl/iwidget.h"
#include "../incl/idraggable.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <utility>

class Node : public IDraggable {
  public:
    Node(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, TTF_Font* font);

    void render(SDL_Renderer* renderer) override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;

    Signal<> onTopButtonClick;
    void     topButtonClick();

    // // dragging signals
    // Signal<>         onHover;
    // Signal<>         onHoverLost;
    // Signal<int, int> onMouseLeftDown;
    // Signal<int, int> onMouseLeftUp;
    // Signal<int, int> onMouseRightDown;
    // Signal<int, int> onMouseRightUp;
    // Signal<int, int> onDragging;

    // inline void changeToBaseColor() noexcept { color = baseColor; }
    // inline void changeToHoverColor() noexcept { color = hoverColor; }

  private:
    int radius;

    // SDL_Color color;
    // SDL_Color hoverColor;
    // SDL_Color baseColor;

    // bool isSelected;
    // bool isHover;
    // bool isDragging;

    Button  button;
    TextBox nameTextBox;
};
