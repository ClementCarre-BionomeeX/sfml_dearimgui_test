#pragma once
#include "../incl/iwidget.h"
#include "../incl/link.h"
#include "../incl/mouse_position.h"
#include "../incl/node.h"
#include "../incl/signal.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <string>

class Canvas : public IWidget, public WidgetManager {

  public:
    Canvas(SDL_Renderer* renderer, TTF_Font* font)
        : IWidget(), WidgetManager(renderer), font(font) {}

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
    void          upLeftNode(Node* node) {
        // if mp_start != node, we add a link
        if (mp_start && mp_start != node) {
            if (!disconnectNodes(mp_start, node)) {
                connectNodes(mp_start, node);
            }
        }
        // remove any active mouse_position if any
        removeAnyMousePosition();
        onNodeLeftUp.emit(node);
    }
    Signal<Node*> onNodeLeftDown;
    void          downLeftNode(Node* node) { onNodeLeftDown.emit(node); }

    Signal<int, int> onBackgroundLeftUp;
    void             backgroundLeftUp(int x, int y) {
        // remove any active mouse_position if any
        removeAnyMousePosition();
        onBackgroundLeftUp.emit(x, y);
    }
    Signal<int, int> onBackgroundLeftDown;
    void             backgroundLeftDown(int x, int y) { onBackgroundLeftDown.emit(x, y); }

    Signal<Node*> onNodeConnectDown;
    void          downConnectNode(Node* node) {
        // create a mouse_pôsition widget
        mp_start = node;
        mp       = addWidget<MousePosition>();
        mp_link =
            addWidget<Link>(node, mp, SDL_Color{50, 50, 50, 255}, SDL_Color{50, 50, 50, 255}, 5);
        onNodeConnectDown.emit(node);
    }

    bool isConnected(IWidget* source, IWidget* target) const noexcept;

  private:
    TTF_Font*      font;
    Node*          mp_start = nullptr;
    Link*          mp_link  = nullptr;
    MousePosition* mp       = nullptr;

    void removeAnyMousePosition() {
        if (mp_link) {
            removeWidget(mp_link);
            mp_link = nullptr;
        }
        if (mp) {
            removeWidget(mp);
            mp = nullptr;
        }
        if (mp_start) {
            mp_start = nullptr;
        }
    }
};
