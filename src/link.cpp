#include "../incl/link.h"
#include "../incl/draw_utils.h"
#include "../incl/utils.h"
#include <cmath>
#include <iostream>

Link::Link(std::weak_ptr<IWidget>  source,
           std::weak_ptr<IWidget>  target,
           std::weak_ptr<Relation> relation,
           int                     thickness)
    : _source(source), _target(target), _relation(relation), _thickness(thickness),
      isHovered(false) {}

void Link::render(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {

    auto locked_relation = _relation.lock();

    auto color = isHovered ? locked_relation->hoverColor() : locked_relation->baseColor();

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, color.r, color.g, color.b, color.a);

    // Scale positions and thickness by zoomFactor
    int zoomedAX        = static_cast<int>((float)a.x * zoomFactor);
    int zoomedAY        = static_cast<int>((float)a.y * zoomFactor);
    int zoomedBX        = static_cast<int>((float)b.x * zoomFactor);
    int zoomedBY        = static_cast<int>((float)b.y * zoomFactor);
    int zoomedThickness = std::max(static_cast<int>((float)_thickness * zoomFactor), 1);

    drawThickLine(renderer, zoomedAX, zoomedAY, zoomedBX, zoomedBY, zoomedThickness, color);
    fillCircle(renderer, zoomedAX, zoomedAY, zoomedThickness / 2);
    fillCircle(renderer, zoomedBX, zoomedBY, zoomedThickness / 2);

    if (locked_relation->directed()) {
        draw_indicator(renderer, color, zoomFactor);
    }
}

void Link::draw_indicator(non_owning_ptr<SDL_Renderer> renderer,
                          SDL_Color const&             color,
                          float                        zoomFactor) const noexcept {
    int arrow_length = static_cast<int>(20.f * zoomFactor);
    int arrow_width  = static_cast<int>(7.f * zoomFactor);

    int    dx    = b.x - a.x;
    int    dy    = b.y - a.y;
    double angle = atan2(dy, dx);

    SDL_Point zoomed_b = {static_cast<int>((float)b.x * zoomFactor),
                          static_cast<int>((float)b.y * zoomFactor)};

    SDL_Point start_point = {
        int(zoomed_b.x - arrow_length * cos(angle)),
        int(zoomed_b.y - arrow_length * sin(angle)),
    };

    SDL_Point left_point = {
        int(start_point.x + arrow_width * cos(angle + 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle + 3.141592 / 2)),
    };
    SDL_Point right_point = {
        int(start_point.x + arrow_width * cos(angle - 3.141592 / 2)),
        int(start_point.y + arrow_width * sin(angle - 3.141592 / 2)),
    };

    drawThickLine(renderer,
                  left_point.x,
                  left_point.y,
                  zoomed_b.x,
                  zoomed_b.y,
                  static_cast<int>((float)_thickness * zoomFactor),
                  color);
    drawThickLine(renderer,
                  zoomed_b.x,
                  zoomed_b.y,
                  right_point.x,
                  right_point.y,
                  static_cast<int>((float)_thickness * zoomFactor),
                  color);
}

void Link::update() {
    auto locked_source = _source.lock();
    auto locked_target = _target.lock();

    a = locked_source->anchor();
    b = locked_target->anchor();

    auto inter_source = find_intersection(locked_source->getRect(), a, b);
    auto inter_target = find_intersection(locked_target->getRect(), a, b);

    if (inter_source) {
        a = inter_source.value();
    }
    if (inter_target) {
        b = inter_target.value();
    }
}

bool Link::handleEvent(SDL_Event& event, float zoomfactor) {
    int mouseX = (int)((float)(event.motion.x) / zoomfactor);
    int mouseY = (int)((float)(event.motion.y) / zoomfactor);
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

bool Link::isExtremity(std::weak_ptr<IWidget> w) const noexcept {
    auto locked_w = w.lock();
    return (locked_w == _source.lock() || locked_w == _target.lock());
}
bool Link::isSource(std::weak_ptr<IWidget> w) const noexcept {
    return w.lock() == _source.lock();
}
bool Link::isTarget(std::weak_ptr<IWidget> w) const noexcept {
    return w.lock() == _target.lock();
}
bool Link::isRelation(std::weak_ptr<Relation> r) const noexcept {
    return r.lock() == _relation.lock();
}
