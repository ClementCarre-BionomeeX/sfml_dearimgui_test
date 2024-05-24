#include "../incl/utils.h"
#include <optional>
#include <utility>

std::optional<SDL_Point> line_intersection(SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4) {

    double denom = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
    if (denom == 0) {
        return {};
    }
    double ua = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / denom;
    if (ua >= 0 && ua <= 1) {
        double ub = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / denom;
        if (ub >= 0 && ub <= 1) {
            int x = int((double)p1.x + ua * (double)(p2.x - p1.x));
            int y = int((double)p1.y + ua * (double)(p2.y - p1.y));
            return std::optional<SDL_Point>({x, y});
        }
    }
    return {};
}

std::optional<SDL_Point> find_intersection(SDL_Rect rect, SDL_Point start, SDL_Point end) {

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

SDL_Color HSVtoRGB(int h, int s, int v) {
    float H = static_cast<float>(h) / 360.0f;
    float S = static_cast<float>(s) / 100.0f;
    float V = static_cast<float>(v) / 100.0f;

    float R{0}, G{0}, B{0};

    int   i = static_cast<int>(H * 6);
    float f = H * 6 - static_cast<float>(i);
    float p = V * (1 - S);
    float q = V * (1 - f * S);
    float t = V * (1 - (1 - f) * S);

    switch (i % 6) {
    case 0:
        R = V, G = t, B = p;
        break;
    case 1:
        R = q, G = V, B = p;
        break;
    case 2:
        R = p, G = V, B = t;
        break;
    case 3:
        R = p, G = q, B = V;
        break;
    case 4:
        R = t, G = p, B = V;
        break;
    case 5:
        R = V, G = p, B = q;
        break;
    }

    return {static_cast<uint8_t>(R * 255.0),
            static_cast<uint8_t>(G * 255.0),
            static_cast<uint8_t>(B * 255.0),
            255};
}
