#include "../incl/link.h"
#include "../incl/draw_utils.h"
#include "../incl/utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <iostream>

Link::Link(IWidget* source, IWidget* target, SDL_Color color, SDL_Color hoverColor, int thickness)
    : source(source), target(target), color(color), hoverColor(hoverColor), thickness(thickness),
      isHovered(false) {}

void Link::render(SDL_Renderer* renderer) {

    SDL_Color currentColor = isHovered ? hoverColor : color;

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
    draw_indicator(renderer, currentColor);
}

void Link::draw_indicator(SDL_Renderer* renderer, SDL_Color color) const noexcept {
    int arrow_length = 20;
    int arrow_width  = 7;

    int    dx    = c - a;
    int    dy    = d - b;
    double angle = atan2(dy, dx);

    SDL_Point start_point = {
        int(c - arrow_length * cos(angle)),
        int(d - arrow_length * sin(angle)),
    };

    SDL_Point left_point = {
        int(start_point.x + arrow_width * cos(angle + 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle + 3.141592 / 2)),
    };
    SDL_Point right_point = {
        int(start_point.x + arrow_width * cos(angle - 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle - 3.141592 / 2)),
    };
    thickLineRGBA(
        renderer, left_point.x, left_point.y, c, d, thickness, color.r, color.g, color.b, color.a);
    thickLineRGBA(renderer,
                  c,
                  d,
                  right_point.x,
                  right_point.y,
                  thickness,
                  color.r,
                  color.g,
                  color.b,
                  color.a);
}

void Link::update() {
    std::tie(a, b) = source->anchor();
    std::tie(c, d) = target->anchor();

    auto inter_source = find_intersection(source->getRect(), {a, b}, {c, d});
    auto inter_target = find_intersection(target->getRect(), {a, b}, {c, d});

    if (inter_source && inter_target) {
        std::tie(a, b) = inter_source.value();
        std::tie(c, d) = inter_target.value();
    }
}

bool Link::handleEvent(SDL_Event& event) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    isHovered  = isNear(mouseX, mouseY);
    return false;
}

bool Link::isNear(int x, int y) const noexcept {
    return pointLineSegmentDistance(x, y) <= thickness / 2;
}

// Function to calculate the distance from point (x, y) to the line segment (x1, y1) - (x2, y2)
double Link::pointLineSegmentDistance(int x, int y) const noexcept {
    double norm2 = (c - a) * (c - a) + (d - b) * (d - b);

    if (norm2 == 0.0) {
        // Segment is a point
        return sqrt((x - a) * (x - a) + (y - b) * (y - b));
    }

    // Project point onto the line defined by the segment
    double u = ((x - a) * (c - a) + (y - b) * (d - b)) / norm2;

    // Clamp u to the 0.0 to 1.0 range to ensure it's within the segment
    if (u < 0.0) {
        u = 0.0;
    }
    if (u > 1.0) {
        u = 1.0;
    }

    // Find the closest point on the line segment
    double closestX = a + u * (c - a);
    double closestY = b + u * (d - b);

    double dist = sqrt((x - closestX) * (x - closestX) + (y - closestY) * (y - closestY));

    // Return the distance from the closest point to the point (x, y)
    return dist;
}

std::pair<int, int> Link::anchor() const noexcept {
    return {(a + c) / 2, (b + d) / 2};
}
