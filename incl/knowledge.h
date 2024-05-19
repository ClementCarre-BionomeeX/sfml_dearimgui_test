#pragma once

#include <SDL2/SDL.h>
#include <tuple>
#include <unordered_map>

enum class KnowledgeState {
    Unknown,
    Uncertified_Acquired,
    Uncertified_Missing,
    Certified_Acquired,
    Certified_Missing
};

// Function to add 30 to each RGB component, ensuring it doesn't exceed 255
SDL_Color getHoverColor(const SDL_Color& color);

// Function to get color and hover color for a given KnowledgeState
std::pair<SDL_Color, SDL_Color> getColor(KnowledgeState state);
