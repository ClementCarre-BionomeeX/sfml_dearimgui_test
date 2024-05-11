#include "../incl/textbutton.h"

void TextButton::render(non_owning_ptr<SDL_Renderer> renderer) {
    // first render the button
    Button::render(renderer);
    SDL_Surface* surface = TTF_RenderUTF8_Solid((TTF_Font*)_font, _text.c_str(), {0, 0, 0, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface((SDL_Renderer*)renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Rect srcRect  = {0, 0, rect.w, rect.h};
    SDL_Rect destRect = {rect.x + 5, rect.y, rect.w - 10, rect.h};
    SDL_RenderCopy((SDL_Renderer*)renderer, texture, &srcRect, &destRect);
    SDL_DestroyTexture(texture);
}
