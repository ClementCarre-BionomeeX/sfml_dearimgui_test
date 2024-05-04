#pragma once

#include "../incl/signal.h"
#include "../incl/textbutton.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class GUI {
  public:
    GUI(SDL_Window* window, int w, TTF_Font* font)
        : window(window), rect{0, 0, w, 0},
          quitButton{10, 10, w - 20, 30, {255, 0, 0, 255}, {200, 50, 50, 255}, 2, "Quit", font},
          addNodeButon{10,
                       50,
                       w - 20,
                       30,
                       {200, 200, 200, 255},
                       {220, 220, 220, 255},
                       2,
                       "Add",
                       font},
          saveButton{10,
                     90,
                     w - 20,
                     30,
                     {180, 180, 180, 255},
                     {220, 220, 220, 255},
                     2,
                     "Save",
                     font},
          loadButton{10,
                     130,
                     w - 20,
                     30,
                     {180, 180, 180, 255},
                     {220, 220, 220, 255},
                     2,
                     "Load",
                     font} {
        update();
        quitButton.onClick.connect([this]() { clickQuit(); });
        addNodeButon.onClick.connect([this]() { clickAddNode(); });
        saveButton.onClick.connect([this]() { clickSave(); });
        loadButton.onClick.connect([this]() { clickLoad(); });
    }

    void render(SDL_Renderer* renderer) {
        // render the background box from top to bottom
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &rect);

        // render all buttons
        quitButton.render(renderer);
        addNodeButon.render(renderer);
        saveButton.render(renderer);
        loadButton.render(renderer);
    }

    bool handleEvent(SDL_Event& event) {
        return quitButton.handleEvent(event) || addNodeButon.handleEvent(event) ||
               saveButton.handleEvent(event) || loadButton.handleEvent(event);
    }

    void update() {
        // get the window size
        SDL_GetWindowSize(window, nullptr, &rect.h);
    }

    Signal<> onQuitClick;
    void     clickQuit() { onQuitClick.emit(); }
    Signal<> onAddNodeClick;
    void     clickAddNode() { onAddNodeClick.emit(); }
    Signal<> onSaveClick;
    void     clickSave() { onSaveClick.emit(); }
    Signal<> onLoadClick;
    void     clickLoad() { onLoadClick.emit(); }

  private:
    SDL_Window* window;
    SDL_Rect    rect;
    TextButton  quitButton;
    TextButton  addNodeButon;
    TextButton  saveButton;
    TextButton  loadButton;
};
