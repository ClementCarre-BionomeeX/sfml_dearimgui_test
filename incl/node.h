#pragma once

#include "../incl/button.h"
#include "../incl/idraggable.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include "../incl/textbutton.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Node : public IDraggable {
  public:
    Node(int                                           x,
         int                                           y,
         int                                           w,
         int                                           h,
         SDL_Color                                     baseColor,
         SDL_Color                                     hoverColor,
         non_owning_ptr<TTF_Font>                      font,
         std::vector<std::shared_ptr<Relation>> const& relationList);

    void render(non_owning_ptr<SDL_Renderer> renderer) override;
    bool handleEvent(SDL_Event& event, float zoomfactor) override;
    void update() override;

    Signal<> onTopButtonClick;
    void     topButtonClick();

    Signal<std::string> onNameChanged;
    void                changeName(std::string name);

    Signal<> onGlobalMouseLeftUp;
    void     globalMouseLeftUp();

    Signal<std::shared_ptr<Relation>> onConnectMouseLeftDown;
    void                              connectMouseLeftDown(std::shared_ptr<Relation> relation);

  private:
    int radius;
    int margin        = 5;
    int topButtonSize = 20;

    TextButton topButton;
    TextBox    nameTextBox;

    std::vector<std::unique_ptr<TextButton>> addConnectionButtonList;

    std::vector<std::shared_ptr<Relation>> const& _relationList;
};
