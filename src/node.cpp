#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor, TTF_Font* font)
    : IDraggable{x, y, w, h, baseColor, hoverColor}, radius{2},
      button(x + 10, y + 10, w - 20, 30, {0, 150, 0, 255}, {0, 250, 100, 255}, 2),
      nameTextBox(x + 10, y + 50, {0, 0, 0, 255}, font, w - 20) {
    button.onClick.connect([&]() { topButtonClick(); });
    nameTextBox.onTextChanged.connect([&](std::string str) { changeName(str); });
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

    button.render(renderer);
    nameTextBox.render(renderer);
}

bool Node::handleEvent(SDL_Event& event) {

    if (button.handleEvent(event)) {
        return true;
    }
    if (nameTextBox.handleEvent(event)) {
        return true;
    }

    // call default Idraggable::handleEvent(event)
    return IDraggable::handleEvent(event);
}

void Node::update() {
    button.moveTo(rect.x + 10, rect.y + 10);
    nameTextBox.moveTo(rect.x + 10, rect.y + 50);
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
