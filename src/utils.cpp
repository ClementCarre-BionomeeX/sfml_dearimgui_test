#include "../incl/utils.h"
#include <optional>
#include <utility>

std::optional<std::pair<int, int>>
line_intersection(SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4) {

    double denom = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
    if (denom == 0) {
        return {};
    }
    double ua = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / denom;
    if (ua >= 0 && ua <= 1) {
        double ub = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / denom;
        if (ub >= 0 && ub <= 1) {
            int x = p1.x + ua * (p2.x - p1.x);
            int y = p1.y + ua * (p2.y - p1.y);
            return std::optional<std::pair<int, int>>(std::make_pair(x, y));
        }
    }
    return {};
}

std::optional<std::pair<int, int>>
find_intersection(SDL_Rect rect, SDL_Point start, SDL_Point end) {

    for (int i = 0; i < 4; ++i) {
        auto iter = line_intersection(
            {rect.x + (i == 1 or i == 2) * rect.w, rect.y + (i == 2 or i == 3) * rect.h},
            {rect.x + (i == 0 or i == 1) * rect.w, rect.y + (i == 1 or i == 2) * rect.h},
            start,
            end);
        if (iter) {
            return iter;
        }
    }
    return {};
}
