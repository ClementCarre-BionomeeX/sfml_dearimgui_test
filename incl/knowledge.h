#pragma once

#include "../incl/json.h"
#include <SDL2/SDL.h>
#include <string>
#include <tuple>
#include <unordered_map>

enum class KnowledgeState {
    Unknown,
    Uncertified_Acquired,
    Uncertified_Missing,
    Certified_Acquired,
    Certified_Missing
};

// Map for string representations of KnowledgeState
const std::unordered_map<KnowledgeState, std::string> KnowledgeStateStrings{
    {KnowledgeState::Unknown, "Unknown"},
    {KnowledgeState::Uncertified_Acquired, "Uncertified_Acquired"},
    {KnowledgeState::Uncertified_Missing, "Uncertified_Missing"},
    {KnowledgeState::Certified_Acquired, "Certified_Acquired"},
    {KnowledgeState::Certified_Missing, "Certified_Missing"}};

// Serialization function
inline nlohmann::json to_json(KnowledgeState const& state) {
    auto it = KnowledgeStateStrings.find(state);
    if (it != KnowledgeStateStrings.end()) {
        return it->second;
    } else {
        return "";
    }
}

// Deserialization function
inline KnowledgeState from_json(nlohmann::json const& j) {
    std::string stateStr = j.get<std::string>();
    for (const auto& pair : KnowledgeStateStrings) {
        if (pair.second == stateStr) {
            return pair.first;
        }
    }
    //? probably should return a kind of error ?
    return KnowledgeState::Unknown;
}

// Function to add 30 to each RGB component, ensuring it doesn't exceed 255
SDL_Color getHoverColor(const SDL_Color& color);

// Function to get color and hover color for a given KnowledgeState
std::pair<SDL_Color, SDL_Color> getColor(KnowledgeState state);
