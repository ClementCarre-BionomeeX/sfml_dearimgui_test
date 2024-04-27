#include "../incl/button.h"
#include "../incl/node.h"
#include "../incl/textbox.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

// MARK: main
int main(/*int argc, char* argv[]*/) {
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
    textBoxes.emplace_back(100, 100, SDL_Color{70, 70, 70, 255}, font, 100);
    textBoxes.emplace_back(100, 150, SDL_Color{0, 200, 200, 255}, font, 150);

    Button quitButton(10,
                      10,
                      50,
                      25,
                      {255, 0, 0, 255},
                      {255, 200, 200, 255},
                      2);    // Red button, lighter red on hover

    Node testnode = Node(50, 300, 100, 100);

    SDL_Event event;
    bool      running = true;
    SDL_StartTextInput();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            for (auto& box : textBoxes) {
                if (box.handleEvent(event)) {
                    continue;
                }
            }
            if (quitButton.handleEvent(event)) {
                running = false;    // Quit if the button is clicked
            }
            if (testnode.handleEvent(event)) {
                // ??
            }
        }

        for (auto& box : textBoxes) {
            box.update();
        }
        quitButton.update();
        testnode.update();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (auto& box : textBoxes) {
            box.render(renderer);
        }

        quitButton.render(renderer);    // Render the button

        testnode.render(renderer);

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
