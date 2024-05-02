#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <limits>
#include <string>

// MARK: TextBox
TextBox::TextBox(int x, int y, SDL_Color color, TTF_Font* font, int minimumTextWidth)
    : IWidget{x, y, 0, 0}, isSelected(false), color(color), texture(nullptr), cursorPosition(0),
      lastCursorBlink(SDL_GetTicks()), cursorVisible(true), minTextWidth(minimumTextWidth),
      font(font) {
    text = "";
}

// MARK: ~TextBox
TextBox::~TextBox() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

// MARK: prepareTextTexture
int TextBox::prepareTextTexture(SDL_Renderer* renderer) {
    const char* renderText = text.empty() ? " " : text.c_str();

    // Create a surface from the text
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, renderText, color);
    texture              = SDL_CreateTextureFromSurface(renderer, surface);

    // Get the width of the rendered text
    int textWidth  = surface->w;
    int textHeight = surface->h;
    SDL_FreeSurface(surface);

    // Set the textbox width and height, ensuring minimum width is respected
    rect.w = std::max(minTextWidth, rect.w);    // Ensure the textbox has at least the minimum width
    rect.h = textHeight;                        // Set the height based on the text height

    return textWidth;
}

// MARK: renderBackground
void TextBox::renderBackground(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);    // White background
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 0, isSelected ? 255 : 0, 255);    // Border color
    SDL_RenderDrawRect(renderer, &rect);
}

// MARK: renderText
void TextBox::renderText(SDL_Renderer* renderer, int w) {
    // Clipping width to ensure text does not overflow
    int displayWidth =
        std::min(w - textOffset, rect.w - 10);    // Assuming 5 pixels padding on each side
    if (displayWidth > 0) {
        SDL_Rect srcRect  = {textOffset, 0, displayWidth, rect.h};
        SDL_Rect destRect = {
            rect.x + 5, rect.y, displayWidth, rect.h};    // Adjusted rect for text display
        SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    }
}

// MARK: render
void TextBox::render(SDL_Renderer* renderer) {
    int w = prepareTextTexture(renderer);
    renderBackground(renderer);
    renderText(renderer, w);
    if (isSelected && cursorVisible) {
        drawCursor(renderer);
    }
}

// MARK: drawCursor
void TextBox::drawCursor(SDL_Renderer* renderer) const {
    int cursorX = rect.x + 5;    // Start cursor at the beginning of the box with padding

    if (cursorPosition > 0 && cursorPosition <= text.length()) {
        std::string  textBeforeCursor = text.substr(0, cursorPosition);
        SDL_Surface* surface          = TTF_RenderText_Solid(font, textBeforeCursor.c_str(), color);
        cursorX += surface->w - textOffset;    // Adjust cursor position by textOffset
        SDL_FreeSurface(surface);
    }

    if (cursorX < rect.x + 5) {    // Ensure cursor does not go out of box on the left
        cursorX = rect.x + 5;
    } else if (cursorX >
               rect.x + rect.w - 5) {    // Ensure cursor does not go out of box on the right
        cursorX = rect.x + rect.w - 5;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);    // Black cursor
    SDL_RenderDrawLine(renderer, cursorX, rect.y + 2, cursorX, rect.y + rect.h - 4);
}

// MARK: update
void TextBox::update() {
    Uint32 time = SDL_GetTicks();
    if (time - lastCursorBlink > 500) {    // Toggle cursor visibility every 500ms
        cursorVisible   = !cursorVisible;
        lastCursorBlink = time;
    }
}

// MARK: handleEvent
bool TextBox::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;
        if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {
            isSelected = true;
            setCursorByClick(x);
            return true;
        } else {
            isSelected = false;
            textOffset = 0;    // Reset text offset when textbox is deselected
            return false;
        }
    } else if (event.type == SDL_KEYDOWN && isSelected) {
        SDL_Keymod mod  = SDL_GetModState();
        bool       ctrl = mod & KMOD_CTRL;
        switch (event.key.keysym.sym) {
        case SDLK_BACKSPACE:
            if (ctrl) {
                deleteWordLeft();
            } else if (cursorPosition > 0 && !text.empty()) {
                text.erase(cursorPosition - 1, 1);
                cursorPosition--;
            }
            updateTextOffsetOnCursorMove();
            break;
        case SDLK_LEFT:
            if (ctrl) {
                moveCursorLeftByWord();
            } else if (cursorPosition > 0) {
                cursorPosition--;
            }
            updateTextOffsetOnCursorMove();
            break;
        case SDLK_RIGHT:
            if (ctrl) {
                moveCursorRightByWord();
            } else if (cursorPosition < text.length()) {
                cursorPosition++;
            }
            updateTextOffsetOnCursorMove();
            break;
        case SDLK_DELETE:
            if (ctrl) {
                deleteWordRight();
            } else if (cursorPosition < text.length()) {
                text.erase(cursorPosition, 1);
            }
            updateTextOffsetOnCursorMove();
            break;
        case SDLK_HOME:
            cursorPosition = 0;
            updateTextOffsetOnCursorMove();
            break;
        case SDLK_END:
            cursorPosition = text.length();
            updateTextOffsetOnCursorMove();
            break;
        }
    } else if (event.type == SDL_TEXTINPUT && isSelected) {
        text.insert(cursorPosition, event.text.text);
        cursorPosition += strlen(event.text.text);
        updateTextOffsetOnCursorMove();
    }
    return isSelected;
}

// MARK: updateTextOffsetOnCursorMove
void TextBox::updateTextOffsetOnCursorMove() {
    std::string textBeforeCursor = text.substr(0, cursorPosition);
    int         cursorTextWidth, textHeight;
    TTF_SizeUTF8(font, textBeforeCursor.c_str(), &cursorTextWidth, &textHeight);

    int visibleTextWidth = rect.w - 10;    // Assuming 5 pixels padding on each side
    int bufferZone       = 10;             // 10 pixels buffer for smoother scrolling

    if (cursorTextWidth < textOffset) {
        // If cursor is left of the current offset, adjust offset to bring the cursor into view
        textOffset = std::max(0, cursorTextWidth - bufferZone);
    } else if (cursorTextWidth < textOffset + bufferZone) {
        // If cursor is within the left buffer zone, reduce offset
        textOffset = std::max(0, cursorTextWidth - bufferZone);
    } else if (cursorTextWidth - textOffset > visibleTextWidth - bufferZone) {
        // If cursor is within the right buffer zone, adjust the offset to the right
        textOffset = cursorTextWidth - (visibleTextWidth - bufferZone);
    }
}

// MARK: setCursorByClick
void TextBox::setCursorByClick(int clickX) {
    int    textWidth  = 0;
    int    height     = 0;
    size_t closestPos = 0;
    int    minDist    = std::numeric_limits<int>::max();

    for (size_t i = 0; i <= text.length(); ++i) {
        std::string subText = text.substr(0, i);
        TTF_SizeText(font, subText.c_str(), &textWidth, &height);

        int currentDist = std::abs((rect.x + textWidth) - clickX);
        if (currentDist < minDist) {
            minDist    = currentDist;
            closestPos = i;
        }
    }

    cursorPosition = closestPos;
}

// MARK: deleteWordLeft
void TextBox::deleteWordLeft() {
    if (cursorPosition == 0)
        return;    // Nothing to delete if at the start
    size_t pos     = cursorPosition;
    bool   inSpace = isspace(text[pos - 1]);

    // Skip whitespace to the left
    while (pos > 0 && isspace(text[pos - 1]))
        pos--;

    if (!inSpace) {
        // Continue to delete until the start of the word
        while (pos > 0 && !isspace(text[pos - 1]))
            pos--;
    }

    text.erase(pos, cursorPosition - pos);
    cursorPosition = pos;    // Move the cursor to the start of the word
}

// MARK: deleteWordRight
void TextBox::deleteWordRight() {
    if (cursorPosition == text.length())
        return;    // Nothing to delete if at the end
    size_t pos     = cursorPosition;
    bool   inSpace = pos < text.length() && isspace(text[pos]);

    // Skip any whitespace at the cursor
    while (pos < text.length() && isspace(text[pos]))
        pos++;

    if (!inSpace) {
        // Delete until the next whitespace or end of text
        while (pos < text.length() && !isspace(text[pos]))
            pos++;
    }

    text.erase(cursorPosition, pos - cursorPosition);
}

// MARK: moveCursorLeftByWord
void TextBox::moveCursorLeftByWord() {
    if (cursorPosition == 0)
        return;
    const char* str = text.c_str();
    size_t      pos = cursorPosition - 1;

    // Skip whitespace and punctuation to the left
    while (pos > 0 && !isalnum(str[pos]))
        pos--;

    // Find the start of the previous word
    while (pos > 0 && isalnum(str[pos - 1]))
        pos--;

    cursorPosition = pos;
}

// MARK: moveCursorRightByWord
void TextBox::moveCursorRightByWord() {
    size_t len = text.length();
    if (cursorPosition == len)
        return;
    const char* str = text.c_str();
    size_t      pos = cursorPosition;

    // Skip whitespace and punctuation to the right
    while (pos < len && !isalnum(str[pos]))
        pos++;

    // Find the end of the current word
    while (pos < len && isalnum(str[pos]))
        pos++;

    cursorPosition = pos;
}
