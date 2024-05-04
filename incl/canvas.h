#pragma once
#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>

class Canvas : public IWidget, public WidgetManager {

  public:
    Canvas(SDL_Renderer* renderer, TTF_Font* font)
        : IWidget(), WidgetManager(renderer), font(font) {}

    // template <typename T, typename... Args>
    // T* addWidget(Args&&... args);

    Node* addNode();
    Node* addNode(int x, int y);
    bool  removeNode(Node* node);
    bool  connectNodes(Node* source, Node* target);
    bool  disconnectNodes(Node* source, Node* target);

    bool                handleEvent(SDL_Event& event) override;
    void                update() override;
    void                render(SDL_Renderer* renderer) override;
    std::pair<int, int> anchor() const noexcept override;

  private:
    // WidgetManager manager;
    TTF_Font* font;
};

// template <typename T, typename... Args>
// inline T* Canvas::addWidget(Args&&... args) {
//     return manager.addWidget<T>(std::forward<Args>(args)...);
// }
