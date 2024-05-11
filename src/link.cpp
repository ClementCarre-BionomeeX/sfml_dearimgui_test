#include "../incl/link.h"
#include "../incl/draw_utils.h"
#include "../incl/utils.h"
#include <cmath>
#include <iostream>

Link::Link(std::shared_ptr<IWidget>  source,
           std::shared_ptr<IWidget>  target,
           std::shared_ptr<Relation> relation,
           int                       thickness)
    : _source(source), _target(target), _relation(relation), _thickness(thickness),
      isHovered(false) {}

void Link::render(non_owning_ptr<SDL_Renderer> renderer) {

    auto color = isHovered ? _relation->baseColor() : _relation->hoverColor();

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, color.r, color.g, color.b, color.a);
    drawThickLine(renderer, a.x, a.y, b.x, b.y, _thickness, color);
    fillCircle(renderer, a.x, a.y, _thickness / 2);
    fillCircle(renderer, b.x, b.y, _thickness / 2);
    if (_relation->directed()) {
        draw_indicator(renderer, color);
    }
}

void Link::draw_indicator(non_owning_ptr<SDL_Renderer> renderer,
                          SDL_Color const&             color) const noexcept {
    int arrow_length = 20;
    int arrow_width  = 7;

    int    dx    = b.x - a.x;
    int    dy    = b.y - a.y;
    double angle = atan2(dy, dx);

    SDL_Point start_point = {
        int(b.x - arrow_length * cos(angle)),
        int(b.y - arrow_length * sin(angle)),
    };

    SDL_Point left_point = {
        int(start_point.x + arrow_width * cos(angle + 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle + 3.141592 / 2)),
    };
    SDL_Point right_point = {
        int(start_point.x + arrow_width * cos(angle - 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle - 3.141592 / 2)),
    };
    drawThickLine(renderer, left_point.x, left_point.y, b.x, b.y, _thickness, color);
    drawThickLine(renderer, b.x, b.y, right_point.x, right_point.y, _thickness, color);
}

void Link::update() {
    a = _source->anchor();
    b = _target->anchor();

    auto inter_source = find_intersection(_source->getRect(), a, b);
    auto inter_target = find_intersection(_target->getRect(), a, b);

    if (inter_source) {
        a = inter_source.value();
    }
    if (inter_target) {
        b = inter_target.value();
    }
}

bool Link::handleEvent(SDL_Event& event) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    isHovered  = isNear(mouseX, mouseY);
    return isHovered;
}

bool Link::isNear(int x, int y) const noexcept {
    return pointLineSegmentDistanceSquared(x, y) <= (_thickness / 2) * (_thickness / 2);
}

// Function to calculate the distance from point (x, y) to the line segment (x1, y1) - (x2, y2)
double Link::pointLineSegmentDistanceSquared(int x, int y) const noexcept {
    double norm2 = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);

    if (norm2 == 0.0) {
        // Segment is a point
        return (x - a.x) * (x - a.x) + (y - a.y) * (y - a.y);
    }

    // Project point onto the line defined by the segment
    double u = ((x - a.x) * (b.x - a.x) + (y - a.y) * (b.y - a.y)) / norm2;

    // clamp u to the 0.0 to 1.0 range to ensure it's within the segment
    if (u < 0.0) {
        u = 0.0;
    }
    if (u > 1.0) {
        u = 1.0;
    }

    // Find the closest point on the line segment
    double closestX = a.x + u * (b.x - a.x);
    double closestY = a.y + u * (b.y - a.y);

    double dist = (x - closestX) * (x - closestX) + (y - closestY) * (y - closestY);

    // Return the distance from the closest point to the point (x, y)
    return dist;
}

SDL_Point Link::anchor() const noexcept {
    return {(a.x + b.x) / 2, (a.y + b.y) / 2};
}

bool Link::isExtremity(std::shared_ptr<IWidget> w) const noexcept {
    return (w == _source || w == _target);
}
bool Link::isSource(std::shared_ptr<IWidget> w) const noexcept {
    return w == _source;
}
bool Link::isTarget(std::shared_ptr<IWidget> w) const noexcept {
    return w == _target;
}
bool Link::isRelation(std::shared_ptr<Relation> r) const noexcept {
    return r == _relation;
}
