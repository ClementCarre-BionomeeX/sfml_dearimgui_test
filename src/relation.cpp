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
