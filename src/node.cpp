#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, TTF_Font* font)
    : IDraggable{x, y, w, h, baseColor, hoverColor}, radius{2}, topButton(x + margin,
                                                                          y + margin,
                                                                          topButtonSize,
                                                                          topButtonSize,
                                                                          {0, 150, 0, 255},
                                                                          {0, 250, 100, 255},
                                                                          2,
                                                                          "X",
                                                                          font),
      nameTextBox(x + margin,
                  y + margin * 2 + topButtonSize,
                  {0, 0, 0, 255},
                  font,
                  w - 2 * margin) {
    topButton.onClick.connect([&]() { topButtonClick(); });
    nameTextBox.onTextChanged.connect([&](std::string str) { changeName(str); });

    // connect all left up and all left down to global
    onMouseLeftUp.connect([&](int x, int y) { globalMouseLeftUp(x, y); });
    topButton.onMouseLeftUp.connect([&](int x, int y) { globalMouseLeftUp(x, y); });
    nameTextBox.onMouseLeftUp.connect([&](int x, int y) { globalMouseLeftUp(x, y); });

    onMouseLeftDown.connect([&](int x, int y) { globalMouseLeftDown(x, y); });
    topButton.onMouseLeftDown.connect([&](int x, int y) { globalMouseLeftDown(x, y); });
    nameTextBox.onMouseLeftDown.connect([&](int x, int y) { globalMouseLeftDown(x, y); });
}

void Node::render(SDL_Renderer* renderer) {
    if (isSelected) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        roundCornerRectangle(renderer, rect, radius);

        int border = 2;
        // draw the background
        SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
        roundCornerRectangle(
            renderer,
            {rect.x + border, rect.y + border, rect.w - 2 * border, rect.h - 2 * border},
            radius);
    } else {
        SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
        roundCornerRectangle(renderer, rect, radius);
    }

    topButton.render(renderer);
    nameTextBox.render(renderer);
}

bool Node::handleEvent(SDL_Event& event) {

    if (topButton.handleEvent(event)) {
        return true;
    }
    if (nameTextBox.handleEvent(event)) {
        return true;
    }

    // call default Idraggable::handleEvent(event)
    return IDraggable::handleEvent(event);
}

void Node::update() {
    topButton.moveTo(rect.x + margin, rect.y + margin);
    nameTextBox.moveTo(rect.x + margin, rect.y + margin * 2 + topButtonSize);
}

void Node::topButtonClick() {
    onTopButtonClick.emit();
}

void Node::changeName(std::string str) {
    onNameChanged.emit(str);
}

void Node::globalMouseLeftUp(int x, int y) {
    onGlobalMouseLeftUp.emit(x, y);
}

void Node::globalMouseLeftDown(int x, int y) {
    onGlobalMouseLeftDown.emit(x, y);
}
