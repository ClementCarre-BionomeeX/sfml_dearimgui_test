#include "../incl/button.h"
#include "../incl/link.h"
#include "../incl/node.h"
#include "../incl/textbox.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

// MARK: main
int main(/*int argc, char* argv[]*/) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window*   window   = SDL_CreateWindow("Text Input Example",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1920,
                                          1080,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("data/FiraCode-Medium.ttf", 16);
    if (!font) {
        SDL_Log("[ERROR] Failed to load font: %s", TTF_GetError());
        return -1;
    }

    SDL_Event event;
    bool      running = true;
    SDL_StartTextInput();

    WidgetManager manager;
    manager.addWidget<TextBox>(100, 100, SDL_Color{70, 70, 70, 255}, font, 100);
    manager.addWidget<TextBox>(100, 150, SDL_Color{0, 200, 200, 255}, font, 150);

    auto* quitButton = manager.addWidget<Button>(
        10, 10, 50, 25, SDL_Color{255, 0, 0, 255}, SDL_Color{255, 200, 200, 255}, 2);
    quitButton->onClick.connect([&running]() { running = false; });

    auto* tn1 = manager.addWidget<Node>(
        50, 300, 100, 200, SDL_Color{0, 200, 230, 255}, SDL_Color{20, 220, 250, 255});
    auto* tn2 = manager.addWidget<Node>(
        200, 300, 100, 200, SDL_Color{0, 150, 180, 255}, SDL_Color{20, 170, 200, 255});

    manager.addWidget<Link>(
        tn1, tn2, SDL_Color{255, 200, 200, 255}, SDL_Color{255, 30, 30, 255}, 11);

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            manager.handleEvents(event);
        }

        manager.updateWidgets();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        manager.renderWidgets(renderer);

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
