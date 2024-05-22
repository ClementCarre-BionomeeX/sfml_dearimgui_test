#include "../incl/relation.h"

std::string Relation::name() const noexcept {
    return _name;
}
SDL_Color Relation::baseColor() const noexcept {
    return _baseColor;
}
SDL_Color Relation::hoverColor() const noexcept {
    return _hoverColor;
}
bool Relation::directed() const noexcept {
    return _directed;
}
bool Relation::transitive() const noexcept {
    return _transitive;
}

bool Relation::operator==(Relation const& other) const noexcept {
    return _name == other._name;
}

json Relation::save() const {
    return {{"name", _name},
            {"basecolor", {_baseColor.r, _baseColor.g, _baseColor.b, _baseColor.a}},
            {"hovercolor", {_hoverColor.r, _hoverColor.g, _hoverColor.b, _hoverColor.a}},
            {"directed", _directed},
            {"transitive", _transitive}};
}

std::tuple<std::string, SDL_Color, SDL_Color, bool, bool> relationFromJson(json j) {
    std::string name = j["name"];

    SDL_Color basecolor{j["basecolor"][0].get<uint8_t>(),
                        j["basecolor"][1].get<uint8_t>(),
                        j["basecolor"][2].get<uint8_t>(),
                        j["basecolor"][3].get<uint8_t>()};
    SDL_Color hovercolor{j["hovercolor"][0].get<uint8_t>(),
                         j["hovercolor"][1].get<uint8_t>(),
                         j["hovercolor"][2].get<uint8_t>(),
                         j["hovercolor"][3].get<uint8_t>()};

    bool directed   = j["directed"];
    bool transitive = j["transitive"];

    return {name, basecolor, hovercolor, directed, transitive};
}
