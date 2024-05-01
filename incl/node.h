#pragma once

#include "../incl/button.h"
#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <utility>

class Node : public IWidget {
  public:
    Node(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor, TTF_Font* font);

    void                render(SDL_Renderer* renderer) override;
    bool                handleEvent(SDL_Event& event) override;
    void                update() override;
    std::pair<int, int> anchor() const noexcept override;

    Signal<> onTopButtonClick;
    // Signal<std::string> onNameChanged;
    void topButtonClick();

  private:
    // using IWidget::rect;
    // SDL_Rect  rect;
    int       radius;
    bool      isHovered;
    SDL_Color color;
    SDL_Color hoverColor;
    bool      isSelected;
    int       startmoveX;
    int       startmoveY;
    Button    button;
    TextBox   nameTextBox;
};
