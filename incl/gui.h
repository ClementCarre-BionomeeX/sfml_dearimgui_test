#pragma once

#include "../incl/non_owning_ptr.h"
#include "../incl/signal.h"
#include "../incl/textbutton.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class GUI {
  public:
    GUI(non_owning_ptr<SDL_Window> window, int w, non_owning_ptr<TTF_Font> font);

    ~GUI();

    void render(non_owning_ptr<SDL_Renderer> renderer);
    bool handleEvent(SDL_Event& event, float zoomfactor);
    void update();

    Signal<> onQuitClick;
    Signal<> onAddNodeClick;
    Signal<> onSaveClick;
    Signal<> onLoadClick;
    Signal<> onFruchtermanClick;
    Signal<> onBackgroundInteraction;
    Signal<> onBackgroundLeftUp;

    void clickQuit();
    void clickAddNode();
    void clickSave();
    void clickLoad();
    void clickFruchterman();
    void interactBackground();
    void leftUpBackground();

  private:
    non_owning_ptr<SDL_Window> _window;
    SDL_Rect                   rect{0, 0, 0, 0};
    TextButton                 quitButton;
    TextButton                 addNodeButon;
    TextButton                 saveButton;
    TextButton                 loadButton;
    TextButton                 fruchtermanButton;
};
