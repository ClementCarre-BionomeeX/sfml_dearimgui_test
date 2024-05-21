#pragma once
#include "../incl/json.h"
#include <SDL2/SDL.h>
#include <string>
using json = nlohmann::json;

class Relation {

  public:
    Relation(std::string name,
             SDL_Color   baseColor,
             SDL_Color   hoverColor,
             bool        directed,
             bool        transitive)
        : _name(name), _baseColor(baseColor), _hoverColor(hoverColor), _directed(directed),
          _transitive(transitive) {}

    std::string name() const noexcept;
    SDL_Color   baseColor() const noexcept;
    SDL_Color   hoverColor() const noexcept;

    bool directed() const noexcept;
    bool transitive() const noexcept;

    bool operator==(Relation const& other) const noexcept;

    json save() const;

  private:
    std::string _name{""};
    SDL_Color   _baseColor{0, 0, 0, 0};
    SDL_Color   _hoverColor{0, 0, 0, 0};

    // TODO
    bool _directed   = true;
    bool _transitive = false;
};
