#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor, TTF_Font* font)
    : IWidget{x, y, w, h}, radius{2}, isHovered(false), color(color), hoverColor(hoverColor),
      isSelected(false), startmoveX(0), startmoveY(0),
      button(x + 10, y + 10, w - 20, 30, {0, 150, 0, 255}, {0, 250, 100, 255}, 2),
      nameTextBox(x + 10, y + 50, {0, 0, 0, 255}, font, w - 20) {
    button.onClick.connect([&]() { topButtonClick(); });
}

void Node::render(SDL_Renderer* renderer) {
    SDL_Color currentColor = isHovered ? hoverColor : color;

    if (isSelected) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        roundCornerRectangle(renderer, rect, radius);

        int border = 2;
        // draw the background
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        roundCornerRectangle(
            renderer,
            {rect.x + border, rect.y + border, rect.w - 2 * border, rect.h - 2 * border},
            radius);
    } else {
        SDL_SetRenderDrawColor(
            renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        roundCornerRectangle(renderer, rect, radius);
    }

    button.render(renderer);
    nameTextBox.render(renderer);
}

bool Node::handleEvent(SDL_Event& event) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;

    if (button.handleEvent(event)) {
        return true;
    }
    if (nameTextBox.handleEvent(event)) {
        return true;
    }

    // Check if the mouse is over the rectangle
    isHovered = (mouseX >= rect.x) && (mouseX <= rect.x + rect.w) && (mouseY >= rect.y) &&
                (mouseY <= rect.y + rect.h);

    // Check the type of event
    bool mouseMove = event.type == SDL_MOUSEMOTION;
    bool leftDown = (event.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_LEFT);
    bool leftUp   = (event.type == SDL_MOUSEBUTTONUP) && (event.button.button == SDL_BUTTON_LEFT);

    // Select the rectangle to start moving
    if (isHovered && leftDown && !isSelected) {
        isSelected = true;
        startmoveX = mouseX - rect.x;    // Distance from the cursor to the rectangle's origin
        startmoveY = mouseY - rect.y;
    }
    // Release the rectangle
    else if (isSelected && leftUp) {
        isSelected = false;
    }
    // Move the rectangle if selected
    else if (isSelected && mouseMove) {
        rect.x = mouseX - startmoveX;
        rect.y = mouseY - startmoveY;
    }

    return isSelected;    // return true if an event is being processed
}

void Node::update() {
    button.moveTo(rect.x + 10, rect.y + 10);
    nameTextBox.moveTo(rect.x + 10, rect.y + 50);
}

std::pair<int, int> Node::anchor() const noexcept {
    return std::pair<int, int>(rect.x + rect.w / 2, rect.y + rect.h / 2);
}

void Node::topButtonClick() {
    onTopButtonClick.emit();
}
