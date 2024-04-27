#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextBox {

  public:
    TextBox(int x, int y, SDL_Color color, TTF_Font* font, int minimumTextWidth);
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& event);
    void update();
    ~TextBox();

  private:
    void drawCursor(SDL_Renderer* renderer) const;
    void setCursorByClick(int clickX);
    void deleteWordLeft();
    void deleteWordRight();
    void moveCursorLeftByWord();
    void moveCursorRightByWord();
    int  prepareTextTexture(SDL_Renderer* renderer);
    void renderBackground(SDL_Renderer* renderer);
    void renderText(SDL_Renderer* renderer, int w);

  private:
    SDL_Rect     rect;
    std::string  text;
    bool         isSelected;
    SDL_Color    color;
    SDL_Texture* texture;
    size_t       cursorPosition;
    Uint32       lastCursorBlink;
    bool         cursorVisible;
    int          minTextWidth;
    TTF_Font*    font;
};
