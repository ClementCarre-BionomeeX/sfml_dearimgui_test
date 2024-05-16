#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <limits>
#include <string>

// MARK: TextBox
TextBox::TextBox(int x, int y, SDL_Color color, non_owning_ptr<TTF_Font> font, int minimumTextWidth)
    : IWidget{x, y, 0, 0}, text{""}, isSelected(false), _color(color),
      // texture(nullptr),
      cursorPosition(0), lastCursorBlink(SDL_GetTicks()), cursorVisible(true),
      minTextWidth(minimumTextWidth), _font(font) {}

// MARK: prepareTextTexture
std::pair<int, non_owning_ptr<SDL_Texture>>
TextBox::prepareTextTexture(non_owning_ptr<SDL_Renderer> renderer) {
    const char* renderText = text.empty() ? " " : text.c_str();

    // Create a surface from the text
    SDL_Surface* surface = TTF_RenderUTF8_Solid((TTF_Font*)_font, renderText, _color);
    auto*        texture = SDL_CreateTextureFromSurface((SDL_Renderer*)renderer, surface);

    // Get the width of the rendered text
    int textWidth  = surface->w;
    int textHeight = surface->h;
    SDL_FreeSurface(surface);

    // Set the textbox width and height, ensuring minimum width is respected
    rect.w = std::max(minTextWidth, rect.w);    // Ensure the textbox has at least the minimum width
    rect.h = textHeight;                        // Set the height based on the text height

    return {textWidth, non_owning_ptr<SDL_Texture>(texture)};
}

void TextBox::render(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {
    auto [w, texture] = prepareTextTexture(renderer);

    // Scale the position and size of the textbox for background rendering
    renderBackground(renderer, zoomFactor);

    // Render the text using the zoom factor
    renderText(renderer, w, texture, zoomFactor);

    // Draw the cursor if the textbox is selected and the cursor is visible
    if (isSelected && cursorVisible) {
        drawCursor(renderer, zoomFactor);
    }
}

void TextBox::renderBackground(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {
    // Scale the background rect
    SDL_Rect zoomedRect = {
        //
        static_cast<int>((float)rect.x * zoomFactor),    //
        static_cast<int>((float)rect.y * zoomFactor),    //
        static_cast<int>((float)rect.w * zoomFactor),    //
        static_cast<int>((float)rect.h * zoomFactor)     //
    };

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, 255, 255, 255, 255);    // White background
    SDL_RenderFillRect((SDL_Renderer*)renderer, &zoomedRect);
    SDL_SetRenderDrawColor(
        (SDL_Renderer*)renderer, 255, 0, isSelected ? 255 : 0, 255);    // Border color
    SDL_RenderDrawRect((SDL_Renderer*)renderer, &zoomedRect);
}

void TextBox::renderText(non_owning_ptr<SDL_Renderer> renderer,
                         int                          w,
                         non_owning_ptr<SDL_Texture>  texture,
                         float                        zoomFactor) {
    // Scale the position and size of the text rect
    int zoomedTextOffset = static_cast<int>((float)textOffset * zoomFactor);
    int zoomedRectW      = static_cast<int>((float)rect.w * zoomFactor);
    int zoomedRectH      = static_cast<int>((float)rect.h * zoomFactor);

    int displayWidth = std::min(w - zoomedTextOffset,
                                zoomedRectW - 10);    // Assuming 5 pixels padding on each side
    if (displayWidth > 0) {
        SDL_Rect srcRect  = {zoomedTextOffset, 0, displayWidth, zoomedRectH};
        SDL_Rect destRect = {static_cast<int>((float)(rect.x + 5) * zoomFactor),
                             static_cast<int>((float)rect.y * zoomFactor),
                             displayWidth,
                             zoomedRectH};    // Adjusted rect for text display
        SDL_RenderCopy((SDL_Renderer*)renderer, (SDL_Texture*)texture, &srcRect, &destRect);
    }
    if (texture) {
        SDL_DestroyTexture((SDL_Texture*)texture);
    }
}

void TextBox::drawCursor(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) const {
    // Calculate the position and size of the cursor based on zoom factor
    int cursorX =
        static_cast<int>((float)(rect.x + 5) *
                         zoomFactor);    // Start cursor at the beginning of the box with padding
    int zoomedRectY = static_cast<int>((float)rect.y * zoomFactor);
    int zoomedRectH = static_cast<int>((float)rect.h * zoomFactor);

    if (cursorPosition > 0 && cursorPosition <= text.length()) {
        std::string  textBeforeCursor = text.substr(0, cursorPosition);
        SDL_Surface* surface =
            TTF_RenderText_Solid((TTF_Font*)_font, textBeforeCursor.c_str(), _color);
        cursorX += static_cast<int>((float)(surface->w - textOffset) *
                                    zoomFactor);    // Adjust cursor position by textOffset
        SDL_FreeSurface(surface);
    }

    int zoomedRectX = static_cast<int>((float)rect.x * zoomFactor);
    int zoomedRectW = static_cast<int>((float)rect.w * zoomFactor);

    if (cursorX <
        zoomedRectX + static_cast<int>(
                          5 * zoomFactor)) {    // Ensure cursor does not go out of box on the left
        cursorX = zoomedRectX + static_cast<int>(5 * zoomFactor);
    } else if (cursorX >
               zoomedRectX + zoomedRectW -
                   static_cast<int>(
                       5 * zoomFactor)) {    // Ensure cursor does not go out of box on the right
        cursorX = zoomedRectX + zoomedRectW - static_cast<int>(5 * zoomFactor);
    }

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, 0, 0, 0, 255);    // Black cursor
    SDL_RenderDrawLine((SDL_Renderer*)renderer,
                       cursorX,
                       zoomedRectY + static_cast<int>(2 * zoomFactor),
                       cursorX,
                       zoomedRectY + zoomedRectH - static_cast<int>(4 * zoomFactor));
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
bool TextBox::handleEvent(SDL_Event& event, float zoomfactor) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = (int)((float)(event.motion.x) / zoomfactor);
        int mouseY = (int)((float)(event.motion.y) / zoomfactor);
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y &&
            mouseY <= rect.y + rect.h) {
            isSelected = true;
            setCursorByClick(mouseX);
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
            onTextChanged.emit(text);
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
            onTextChanged.emit(text);
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
        onTextChanged.emit(text);
    }
    return isSelected;
}

// MARK: updateTextOffsetOnCursorMove
void TextBox::updateTextOffsetOnCursorMove() {
    std::string textBeforeCursor = text.substr(0, cursorPosition);
    int         cursorTextWidth, textHeight;
    TTF_SizeUTF8((TTF_Font*)_font, textBeforeCursor.c_str(), &cursorTextWidth, &textHeight);

    int visibleTextWidth = rect.w - 10;    // Assuming 5 pixels padding on each side
    int bufferZone       = 5;              // 10 pixels buffer for smoother scrolling

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
        TTF_SizeText((TTF_Font*)_font, subText.c_str(), &textWidth, &height);

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
    onTextChanged.emit(text);
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
    onTextChanged.emit(text);
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

void TextBox::changeText(std::string str) {
    text = str;
    onTextChanged.emit(text);
}
