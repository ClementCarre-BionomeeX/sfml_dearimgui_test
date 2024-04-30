#include "../incl/link.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <iostream>

Link::Link(IWidget* source, IWidget* target, SDL_Color color, SDL_Color hoverColor, int thickness)
    : source(source), target(target), color(color), hoverColor(hoverColor), thickness(thickness),
      isHovered(false) {}

void Link::render(SDL_Renderer* renderer) {

    SDL_Color currentColor = isHovered ? hoverColor : color;
    auto [a, b]            = source->anchor();
    auto [c, d]            = target->anchor();

    SDL_SetRenderDrawColor(
        renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    thickLineRGBA(renderer,
                  a,
                  b,
                  c,
                  d,
                  thickness,
                  currentColor.r,
                  currentColor.g,
                  currentColor.b,
                  currentColor.a);
    fillCircle(renderer, a, b, thickness / 2);
    fillCircle(renderer, c, d, thickness / 2);
}

void Link::update() {}

bool Link::handleEvent(SDL_Event& event) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    isHovered  = isNear(mouseX, mouseY);
    return false;
}

bool Link::isNear(int x, int y) {
    return pointLineSegmentDistance(x, y) <= thickness / 2;
}

// Function to calculate the distance from point (x, y) to the line segment (x1, y1) - (x2, y2)
double Link::pointLineSegmentDistance(int x, int y) {
    auto [x1, y1] = source->anchor();
    auto [x2, y2] = target->anchor();

    double norm2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);

    if (norm2 == 0.0) {
        // Segment is a point
        return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
    }

    // Project point onto the line defined by the segment
    double u = ((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / norm2;

    // Clamp u to the 0.0 to 1.0 range to ensure it's within the segment
    if (u < 0.0) {
        u = 0.0;
    }
    if (u > 1.0) {
        u = 1.0;
    }

    // Find the closest point on the line segment
    double closestX = x1 + u * (x2 - x1);
    double closestY = y1 + u * (y2 - y1);

    double dist = sqrt((x - closestX) * (x - closestX) + (y - closestY) * (y - closestY));

    // Return the distance from the closest point to the point (x, y)
    return dist;
}

std::pair<int, int> Link::anchor() const noexcept {
    auto [a, b] = source->anchor();
    auto [c, d] = target->anchor();

    return {(a + c) / 2, (b + d) / 2};
}
