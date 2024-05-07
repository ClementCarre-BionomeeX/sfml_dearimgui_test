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
