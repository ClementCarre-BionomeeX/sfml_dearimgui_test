#pragma once
#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>

class Canvas : IWidget {

  public:
    Canvas(SDL_Renderer* renderer) : manager(renderer) {}

    bool addNode();
    bool removeNode(Node* node);
    bool connectNodes(Node* source, Node* target);
    bool disconnectNodes(Node* source, Node* target);

    bool                handleEvent(SDL_Event& event);
    void                update();
    void                render(SDL_Renderer* renderer);
    std::pair<int, int> anchor() const noexcept;
    // void                moveTo(int x, int y) noexcept;

  private:
    WidgetManager manager;
    IWidget*      selection;
    int           startx;
    int           starty;
};
