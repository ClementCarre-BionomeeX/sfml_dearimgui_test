#pragma once
#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/signal.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <string>

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

    Signal<Node*> onNodeLeftUp;
    void          upLeftNode(Node* node) { onNodeLeftUp.emit(node); }
    Signal<Node*> onNodeLeftDown;
    void          downLeftNode(Node* node) { onNodeLeftDown.emit(node); }

    Signal<int, int> onBackgroundLeftUp;
    void             backgroundLeftUp(int x, int y) { onBackgroundLeftUp.emit(x, y); }
    Signal<int, int> onBackgroundLeftDown;
    void             backgroundLeftDown(int x, int y) { onBackgroundLeftDown.emit(x, y); }

  private:
    // WidgetManager manager;
    TTF_Font* font;
};

// template <typename T, typename... Args>
// inline T* Canvas::addWidget(Args&&... args) {
//     return manager.addWidget<T>(std::forward<Args>(args)...);
// }
