#include "../incl/textbutton.h"

void TextButton::render(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {
    // Scale the button's position and size by the zoomFactor
    SDL_Rect zoomedRect = {static_cast<int>(static_cast<float>(rect.x) * zoomFactor),
                           static_cast<int>(static_cast<float>(rect.y) * zoomFactor),
                           static_cast<int>(static_cast<float>(rect.w) * zoomFactor),
                           static_cast<int>(static_cast<float>(rect.h) * zoomFactor)};

    // Render the button first (assuming Button::render handles scaling internally)
    Button::render(renderer, zoomFactor);

    // Create the surface and texture for the text
    SDL_Surface* surface = TTF_RenderUTF8_Solid(_font.get(), _text.c_str(), {0, 0, 0, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get(), surface);
    SDL_FreeSurface(surface);

    // Scale the source and destination rectangles
    SDL_Rect srcRect  = {0, 0, surface->w, surface->h};
    SDL_Rect destRect = {zoomedRect.x + static_cast<int>(5 * zoomFactor),
                         zoomedRect.y,
                         zoomedRect.w - static_cast<int>(10 * zoomFactor),
                         zoomedRect.h};

    SDL_RenderCopy(renderer.get(), texture, &srcRect, &destRect);
    SDL_DestroyTexture(texture);
}
