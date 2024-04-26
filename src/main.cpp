#include "../incl/button.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

// MARK: TextBox
struct TextBox {
    SDL_Rect     rect;
    std::string  text;
    bool         isSelected;
    SDL_Color    color;
    SDL_Texture* texture;
    size_t       cursorPosition;
    Uint32       lastCursorBlink;
    bool         cursorVisible;
    int          minTextWidth;    // Define a minimum width for the text box

    TextBox(int x, int y, SDL_Color color, int minimumTextWidth)
        : rect{x, y, 0, 0}, isSelected(false), color(color), texture(nullptr), cursorPosition(0),
          lastCursorBlink(SDL_GetTicks()), cursorVisible(true), minTextWidth(minimumTextWidth) {
        text = "";
    }

    ~TextBox() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    void render(SDL_Renderer* renderer, TTF_Font* font) {
        // Check if text is empty and handle rendering accordingly
        const char* renderText =
            text.empty() ? " " : text.c_str();    // Use space only for rendering purposes

        SDL_Surface* surface = TTF_RenderText_Solid(font, renderText, color);
        texture              = SDL_CreateTextureFromSurface(renderer, surface);

        // Adjust the width of the rectangle to have padding and minimum width
        rect.w = surface->w + 10;    // Add padding for the cursor
        if (rect.w < minTextWidth) {
            rect.w = minTextWidth;    // Ensure a minimum width
        }
        rect.h = surface->h;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);    // Set the background color
        SDL_RenderFillRect(renderer, &rect);                     // Fill the background
        SDL_SetRenderDrawColor(renderer,
                               255,
                               0,
                               isSelected ? 255 : 0,
                               255);            // Color for the text or selection indicator
        SDL_RenderDrawRect(renderer, &rect);    // Draw the rectangle border if needed

        // Render the text at a position within the rectangle (center vertically)
        SDL_Rect textRect = {rect.x + 5, rect.y, surface->w, rect.h};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        if (isSelected && cursorVisible) {
            drawCursor(renderer, font);
        }
    }

    void drawCursor(SDL_Renderer* renderer, TTF_Font* font) {
        int cursorX = rect.x + 2;    // Start cursor at the beginning of the box
        if (cursorPosition > 0 && cursorPosition <= text.length()) {
            std::string  textBeforeCursor = text.substr(0, cursorPosition);
            SDL_Surface* surface = TTF_RenderText_Solid(font, textBeforeCursor.c_str(), color);
            cursorX += surface->w;
            SDL_FreeSurface(surface);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);    // Black cursor
        SDL_RenderDrawLine(renderer, cursorX, rect.y + 2, cursorX, rect.y + rect.h - 4);
    }

    void update() {
        Uint32 time = SDL_GetTicks();
        if (time - lastCursorBlink > 500) {    // Toggle cursor visibility every 500ms
            cursorVisible   = !cursorVisible;
            lastCursorBlink = time;
        }
    }

    bool handleEvent(SDL_Event& event, TTF_Font* font) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {
                isSelected = true;
                setCursorByClick(x, font);
                return true;
            } else {
                isSelected = false;
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
                break;
            case SDLK_LEFT:
                if (ctrl) {
                    moveCursorLeftByWord();
                } else if (cursorPosition > 0) {
                    cursorPosition--;
                }
                break;
            case SDLK_RIGHT:
                if (ctrl) {
                    moveCursorRightByWord();
                } else if (cursorPosition < text.length()) {
                    cursorPosition++;
                }
                break;
            case SDLK_DELETE:
                if (ctrl) {
                    deleteWordRight();
                } else if (cursorPosition < text.length()) {
                    text.erase(cursorPosition, 1);
                }
                break;
            case SDLK_HOME:
                cursorPosition = 0;
                break;
            case SDLK_END:
                cursorPosition = text.length();
                break;
            }
        } else if (event.type == SDL_TEXTINPUT && isSelected) {
            text.insert(cursorPosition, event.text.text);
            cursorPosition += strlen(event.text.text);
        }
        return false;
    }

    void setCursorByClick(int clickX, TTF_Font* font) {
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
    void deleteWordLeft() {
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

    void deleteWordRight() {
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

    void moveCursorLeftByWord() {
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

    void moveCursorRightByWord() {
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
};

// MARK: main
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "Text Input Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("data/FiraCode-Medium.ttf", 24);
    if (!font) {
        SDL_Log("[ERROR] Failed to load font: %s", TTF_GetError());
        return -1;
    }

    std::vector<TextBox> textBoxes;
    textBoxes.emplace_back(100, 100, SDL_Color{70, 70, 70, 255}, 100);
    textBoxes.emplace_back(100, 150, SDL_Color{0, 200, 200, 255}, 150);

    Button quitButton(10,
                      10,
                      50,
                      25,
                      {255, 0, 0, 255},
                      {255, 200, 200, 255},
                      2);    // Red button, lighter red on hover

    SDL_Event event;
    bool      running = true;
    SDL_StartTextInput();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            for (auto& box : textBoxes) {
                if (box.handleEvent(event, font)) {
                    continue;
                }
            }
            if (quitButton.handleEvent(event)) {
                running = false;    // Quit if the button is clicked
            }
        }

        for (auto& box : textBoxes) {
            box.update();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& box : textBoxes) {
            box.render(renderer, font);
        }

        quitButton.render(renderer);    // Render the button

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
