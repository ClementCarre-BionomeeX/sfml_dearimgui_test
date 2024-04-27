#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor)
    : rect{x, y, w, h}, radius{2}, isHovered(false), color(color), hoverColor(hoverColor) {}

void Node::render(SDL_Renderer* renderer) {
    SDL_Color currentColor = isHovered ? hoverColor : color;

    // draw the background
    SDL_SetRenderDrawColor(
        renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    SDL_Rect middleRect = {rect.x + radius, rect.y, rect.w - 2 * radius, rect.h};
    SDL_RenderFillRect(renderer, &middleRect);
    // Top and bottom strips (to fill the gaps in the rounded corners)
    SDL_Rect topRect = {rect.x, rect.y + radius, rect.w, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &topRect);

    // Four corners: top-left, top-right, bottom-left, bottom-right
    fillCircle(renderer, rect.x + radius, rect.y + radius, radius);
    fillCircle(renderer, rect.x + rect.w - radius - 1, rect.y + radius, radius);
    fillCircle(renderer, rect.x + radius, rect.y + rect.h - radius - 1, radius);
    fillCircle(renderer, rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius);
}

bool Node::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        isHovered  = (mouseX >= rect.x) && (mouseX <= rect.x + rect.w) && (mouseY >= rect.y) &&
                    (mouseY <= rect.y + rect.h);
    }
}

void Node::update() {}
