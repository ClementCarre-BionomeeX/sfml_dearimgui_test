#include "../incl/knowledge.h"

SDL_Color getHoverColor(const SDL_Color& color) {
    int r, g, b, a;
    std::tie(r, g, b, a) = std::tie(color.r, color.g, color.b, color.a);
    r                    = std::min(r + 50, 255);
    g                    = std::min(g + 50, 255);
    b                    = std::min(b + 50, 255);
    return {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
}

std::pair<SDL_Color, SDL_Color> getColor(KnowledgeState state) {
    // Map to associate each KnowledgeState with its color
    static std::unordered_map<KnowledgeState, SDL_Color> colorMap = {
        {
            KnowledgeState::Certified_Acquired,    //
            {82, 220, 69, 255}                     //
        },
        {
            KnowledgeState::Uncertified_Acquired,    //
            {86, 227, 196, 255}                      //
        },
        {
            KnowledgeState::Unknown,    //
            {170, 173, 178, 255}        //
        },
        {
            KnowledgeState::Uncertified_Missing,    //
            {237, 190, 109, 255}                    //
        },
        {
            KnowledgeState::Certified_Missing,    //
            {246, 73, 73, 255}                    //
        }};

    SDL_Color color      = colorMap[state];
    SDL_Color hoverColor = getHoverColor(color);
    return std::make_pair(color, hoverColor);
}
