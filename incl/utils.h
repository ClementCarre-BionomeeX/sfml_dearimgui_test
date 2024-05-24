#pragma once
#include "../incl/json.h"
#include <SDL2/SDL.h>
#include <optional>
#include <utility>
using json = nlohmann::json;

std::optional<SDL_Point> line_intersection(SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Point p4);

std::optional<SDL_Point> find_intersection(SDL_Rect rect, SDL_Point start, SDL_Point end);

SDL_Color HSVtoRGB(int h, int s, int v);
