#include "../incl/label.h"

Label::Label(int                x,
             int                y,
             int                w,
             int                h,
             std::string const& _text,
             SDL_Color          _color,
             TTF_Font*          _font)
    : IWidget{x, y, w, h}, text(_text), color(_color), font(_font), texture(nullptr) {}

Label::~Label() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Label::setText(const std::string& newText) {
    text = newText;
}

void Label::setColor(SDL_Color newColor) {
    color = newColor;
}

void Label::render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) {

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    texture              = SDL_CreateTextureFromSurface((SDL_Renderer*)renderer, surface);

    SDL_Rect renderingRect = {
        //
        (int)((float)(rect.x) * zoomfactor),    //
        (int)((float)(rect.y) * zoomfactor),    //
        (int)((float)(rect.w) * zoomfactor),    //
        (int)((float)(rect.h) * zoomfactor)     //
    };

    // Render the texture
    SDL_RenderCopy((SDL_Renderer*)renderer, texture, nullptr, &renderingRect);
    SDL_FreeSurface(surface);
}
