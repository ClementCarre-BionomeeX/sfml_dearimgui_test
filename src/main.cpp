#include "../incl/button.h"
#include "../incl/canvas.h"
#include "../incl/gui.h"
#include "../incl/link.h"
#include "../incl/node.h"
#include "../incl/textbox.h"
#include "../incl/widgetmanager.h"
// #include "../nativefiledialog/src/include/nfd.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cctype>
#include <gtk/gtk.h>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../incl/filedialog.h"
#include "../incl/relation.h"

// MARK: main
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    gtk_init(&argc, &argv);

    SDL_Window*   window   = SDL_CreateWindow("Text Input Example",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1920,
                                          1080,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("data/FiraCode-Medium.ttf", 12);
    if (!font) {
        SDL_Log("[ERROR] Failed to load font: %s", TTF_GetError());
        return -1;
    }

    SDL_Event event{0};
    bool      running = true;
    SDL_StartTextInput();

    // std::vector<Relation> relations{{"Is A", {0, 200, 0, 255}, {50, 250, 50, 255}}};

    Canvas canvas{non_owning_ptr<SDL_Renderer>(renderer), non_owning_ptr<TTF_Font>(font)};

    // canvas.onNodeLeftUp.connect(
    //     [&](IWidget* widget) { std::cout << "Node Left Up from " << widget << std::endl; });
    // canvas.onNodeConnectDown.connect([&](IWidget* widget) {
    //     std::cout << "Node Left Down for connection from " << widget << std::endl;
    // });

    // canvas.onBackgroundLeftUp.connect([&](int x, int y) {
    //     std::cout << "Background Left Up at (" << x << "x" << y << ")" << std::endl;
    // });
    // canvas.onBackgroundLeftDown.connect([&](int x, int y) {
    //     std::cout << "Background Left Down (" << x << "x" << y << ")" << std::endl;
    // });

    // auto* tn1 = canvas.addNode(300, 50);
    // auto* tn2 = canvas.addNode(450, 50);
    // auto* tn3 = canvas.addNode(300, 300);

    // canvas.connectNodes(tn1, tn2);
    // canvas.connectNodes(tn3, tn2);

    GUI gui{non_owning_ptr<SDL_Window>(window), 100, non_owning_ptr<TTF_Font>(font)};
    gui.onQuitClick.connect([&running]() { running = false; });
    gui.onAddNodeClick.connect([&canvas]() { canvas.addNode(200, 50); });

    gui.onLoadClick.connect([]() {
        std::string result;
        LoadFileDialog(result);
        if (result != "") {
            std::cout << result << std::endl;
        } else {
            std::cout << "no path provided" << std::endl;
        }
    });
    gui.onSaveClick.connect([]() {
        std::string result;
        SaveFileDialog(result);
        if (result != "") {
            std::cout << result << std::endl;
        } else {
            std::cout << "no path provided" << std::endl;
        }
    });

    gui.onBackgroundLeftUp.connect([&canvas]() { canvas.backgroundLeftUp(0, 0); });

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // first, catch GUI event
            if (!gui.handleEvent(event)) {
                // then other events if no GUI
                canvas.handleEvent(event);
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
        canvas.render(non_owning_ptr<SDL_Renderer>(renderer));
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
