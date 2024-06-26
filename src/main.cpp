#include "../incl/button.h"
#include "../incl/canvas.h"
#include "../incl/filedialog.h"
#include "../incl/gui.h"
#include "../incl/json.h"
#include "../incl/link.h"
#include "../incl/node.h"
#include "../incl/relation.h"
#include "../incl/textbox.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
using json = nlohmann::json;

// MARK: main
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    gtk_init(&argc, &argv);

    SDL_Window*   window   = SDL_CreateWindow("Node Editor",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1920,
                                          1080,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("data/FiraCode-Medium.ttf", 64);
    if (!font) {
        SDL_Log("[ERROR] Failed to load font: %s", TTF_GetError());
        return -1;
    }

    SDL_Event event{0};
    bool      running = true;
    SDL_StartTextInput();

    Canvas canvas{non_owning_ptr<SDL_Renderer>(renderer), non_owning_ptr<TTF_Font>(font)};

    GUI gui{non_owning_ptr<SDL_Window>(window), 100, non_owning_ptr<TTF_Font>(font)};
    gui.onQuitClick.connect([&running]() { running = false; });
    gui.onAddNodeClick.connect([&canvas]() {
        canvas.addNode(static_cast<int>(200.0 / canvas.getZoomFactor()),
                       static_cast<int>(50.0 / canvas.getZoomFactor()));
    });

    gui.onLoadClick.connect([&canvas]() {
        std::string result;
        LoadFileDialog(result);
        if (result != "") {
            std::cout << result << std::endl;
        } else {
            std::cout << "no path provided" << std::endl;
        }
        canvas.load(result);
    });
    gui.onSaveClick.connect([&canvas]() {
        std::string result;
        SaveFileDialog(result);
        if (result != "") {
            std::cout << result << std::endl;
        } else {
            std::cout << "no path provided" << std::endl;
        }
        std::ofstream file(result);
        file << canvas.save().dump();
    });

    gui.onFruchtermanClick.connect([&canvas, renderer]() {
        canvas.applyFruchtermanReingoldAlgorithm(non_owning_ptr<SDL_Renderer>(renderer));
    });

    gui.onBackgroundLeftUp.connect([&canvas]() { canvas.backgroundLeftUp(0, 0); });

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // first, catch GUI event
            if (!gui.handleEvent(event, 1.0)) {
                // then other events if no GUI
                canvas.handleEvent(event, 1.0);
            }
        }

        //? updates
        // first update widgets
        canvas.update();
        // then GUI
        gui.update();

        //? rendering
        // render Background
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        SDL_RenderClear(renderer);
        // then render widgets
        canvas.render(non_owning_ptr<SDL_Renderer>(renderer), 1.f);
        // render GUI last
        gui.render(non_owning_ptr<SDL_Renderer>(renderer));

        SDL_RenderPresent(renderer);

        //? add delay for 60 fps, if necessary
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
