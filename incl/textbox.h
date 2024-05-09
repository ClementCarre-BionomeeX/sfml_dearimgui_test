#pragma once

#include "../incl/iwidget.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <utility>

class TextBox : public IWidget {

  public:
    TextBox(int x, int y, SDL_Color color, TTF_Font* font, int minimumTextWidth);

    void render(non_owning_ptr<SDL_Renderer> renderer) override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;

    Signal<std::string> onTextChanged;
    void                changeText(std::string str);

  private:
    void                         drawCursor(non_owning_ptr<SDL_Renderer> renderer) const;
    void                         setCursorByClick(int clickX);
    void                         deleteWordLeft();
    void                         deleteWordRight();
    void                         moveCursorLeftByWord();
    void                         moveCursorRightByWord();
    std::pair<int, SDL_Texture*> prepareTextTexture(non_owning_ptr<SDL_Renderer> renderer);
    void                         renderBackground(non_owning_ptr<SDL_Renderer> renderer);
    void renderText(non_owning_ptr<SDL_Renderer> renderer, int w, SDL_Texture* texture);
    void updateTextOffsetOnCursorMove();

  private:
    std::string text;
    bool        isSelected;
    SDL_Color   _color;
    size_t      cursorPosition;
    Uint32      lastCursorBlink;
    bool        cursorVisible;
    int         minTextWidth;
    TTF_Font*   _font;
    int         textOffset = 0;
};
