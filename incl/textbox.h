#pragma once

#include "../incl/iwidget.h"
#include "../incl/non_owning_ptr.h"
#include "../incl/signal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <utility>

class TextBox : public IWidget {

  public:
    TextBox(int x, int y, SDL_Color color, non_owning_ptr<TTF_Font> font, int minimumTextWidth);

    ~TextBox() { onTextChanged.disconnect_all(); }

    void render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    bool handleEvent(SDL_Event& event, float zoomfactor) override;
    void update() override;

    Signal<std::string> onTextChanged;
    void                changeText(std::string str);

    int getHeight() const;

  private:
    void setCursorByClick(int clickX);
    void deleteWordLeft();
    void deleteWordRight();
    void moveCursorLeftByWord();
    void moveCursorRightByWord();

    std::pair<int, non_owning_ptr<SDL_Texture>>
    prepareTextTexture(non_owning_ptr<SDL_Renderer> renderer);

    void renderBackground(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor);
    void renderText(non_owning_ptr<SDL_Renderer> renderer,
                    int                          w,
                    non_owning_ptr<SDL_Texture>  texture,
                    float                        zoomfactor);
    void drawCursor(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) const;

    void updateTextOffsetOnCursorMove();

  private:
    std::string              text;
    bool                     isSelected;
    SDL_Color                _color;
    size_t                   cursorPosition;
    Uint32                   lastCursorBlink;
    bool                     cursorVisible;
    int                      minTextWidth;
    non_owning_ptr<TTF_Font> _font;
    int                      textOffset = 0;
};
