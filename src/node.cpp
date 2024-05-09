#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int                                           x,
           int                                           y,
           int                                           w,
           int                                           h,
           SDL_Color                                     baseColor,
           SDL_Color                                     hoverColor,
           TTF_Font*                                     font,
           std::vector<std::shared_ptr<Relation>> const& relationList)
    : IDraggable{x, y, w, h, baseColor, hoverColor}, radius{2}, topButton(x + margin,
                                                                          y + margin,
                                                                          topButtonSize,
                                                                          topButtonSize,
                                                                          {200, 0, 0, 255},
                                                                          {250, 50, 50, 255},
                                                                          2,
                                                                          "X",
                                                                          font),
      nameTextBox(x + margin, y + margin * 2 + topButtonSize, {0, 0, 0, 255}, font, w - 2 * margin),
      _relationList(relationList) {
    topButton.onClick.connect([&]() { topButtonClick(); });
    nameTextBox.onTextChanged.connect([&](std::string str) { changeName(str); });

    // connect all left up and all left down to global
    onMouseLeftUp.connect([&](int, int) { globalMouseLeftUp(); });
    topButton.onMouseLeftUp.connect([&](int, int) { globalMouseLeftUp(); });
    nameTextBox.onMouseLeftUp.connect([&](int, int) { globalMouseLeftUp(); });

    for (std::size_t i = 0; i < relationList.size(); ++i) {
        auto ptr = relationList[i];
        addConnectionButtonList.emplace_back(std::make_unique<TextButton>(x + margin,
                                                                          y,
                                                                          w - 2 * margin,
                                                                          30,
                                                                          ptr->baseColor(),
                                                                          ptr->hoverColor(),
                                                                          2,
                                                                          ptr->name(),
                                                                          font));
    }

    std::size_t i = 0;
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->onMouseLeftUp.connect([&](int, int) { globalMouseLeftUp(); });
        connectionButton->onMouseLeftDown.connect(
            [&, relationList, i](int, int) { connectMouseLeftDown(relationList[i]); });
        i++;
    }
}

void Node::render(non_owning_ptr<SDL_Renderer> renderer) {
    if (isSelected) {
        SDL_SetRenderDrawColor((SDL_Renderer*)renderer, 255, 215, 0, 255);
        roundCornerRectangle(renderer, rect, radius);

        int border = 2;
        // draw the background
        SDL_SetRenderDrawColor((SDL_Renderer*)renderer, _color->r, _color->g, _color->b, _color->a);
        roundCornerRectangle(
            renderer,
            {rect.x + border, rect.y + border, rect.w - 2 * border, rect.h - 2 * border},
            radius);
    } else {
        SDL_SetRenderDrawColor((SDL_Renderer*)renderer, _color->r, _color->g, _color->b, _color->a);
        roundCornerRectangle(renderer, rect, radius);
    }

    topButton.render(renderer);
    nameTextBox.render(renderer);
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->render(renderer);
    }
}

bool Node::handleEvent(SDL_Event& event) {

    bool handled = false;

    for (auto& connectionButton : addConnectionButtonList) {
        handled |= connectionButton->handleEvent(event);
    }

    if (topButton.handleEvent(event) || nameTextBox.handleEvent(event) || handled) {
        return true;
    }

    // call default Idraggable::handleEvent(event)
    return IDraggable::handleEvent(event);
}

void Node::update() {
    topButton.moveTo(rect.x + margin, rect.y + margin);
    nameTextBox.moveTo(rect.x + margin, rect.y + margin * 2 + topButtonSize);
    SDL_Rect ntb_rect = nameTextBox.getRect();

    int i = 0;
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->moveTo(
            rect.x + margin, rect.y + 3 * margin + topButtonSize + ntb_rect.h + i * (margin + 30));
        i++;
    }
    rect.h = 3 * margin + topButtonSize + ntb_rect.h +
             (int)addConnectionButtonList.size() * (30 + margin);
}

void Node::topButtonClick() {
    onTopButtonClick.emit();
}

void Node::changeName(std::string str) {
    onNameChanged.emit(str);
}

void Node::globalMouseLeftUp() {
    onGlobalMouseLeftUp.emit();
}

void Node::connectMouseLeftDown(std::shared_ptr<Relation> relation) {
    onConnectMouseLeftDown.emit(relation);
}
