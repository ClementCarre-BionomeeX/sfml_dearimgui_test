#include "../incl/label.h"

Label::Label(int                      x,
             int                      y,
             int                      w,
             int                      h,
             std::string const&       _text,
             SDL_Color                _color,
             non_owning_ptr<TTF_Font> _font)
    : IWidget{x, y, w, h}, text(_text), color(_color), font(_font) {}

Label::~Label() {}

void Label::setText(std::string const& newText) {
    text = newText;
}

std::string Label::getText() const {
    return text;
}

void Label::setColor(SDL_Color newColor) {
    color = newColor;
}

void Label::render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) {

    SDL_Surface* surface = TTF_RenderText_Blended(font.get(), text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get(), surface);

    SDL_Rect renderingRect = {
        //
        static_cast<int>(static_cast<float>(rect.x) * zoomfactor),    //
        static_cast<int>(static_cast<float>(rect.y) * zoomfactor),    //
        static_cast<int>(static_cast<float>(rect.w) * zoomfactor),    //
        static_cast<int>(static_cast<float>(rect.h) * zoomfactor)     //
    };

    // Render the texture
    SDL_RenderCopy(renderer.get(), texture, nullptr, &renderingRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
