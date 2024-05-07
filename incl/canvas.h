#pragma once
#include "../incl/iwidget.h"
#include "../incl/link.h"
#include "../incl/mouse_position.h"
#include "../incl/node.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <string>

class Canvas : public IWidget, public WidgetManager {

  public:
    Canvas(SDL_Renderer* renderer, TTF_Font* font)
        : IWidget(), WidgetManager(renderer), _font(font), vec() {
        vec.push_back(std::make_shared<Relation>(
            "Is A", SDL_Color{0, 200, 0, 255}, SDL_Color{30, 230, 30, 255}, false, true));
        vec.push_back(std::make_shared<Relation>("Example Of",
                                                 SDL_Color{100, 100, 200, 255},
                                                 SDL_Color{130, 130, 230, 255},
                                                 true,
                                                 true));
        vec.push_back(std::make_shared<Relation>(
            "Explain", SDL_Color{200, 200, 0, 255}, SDL_Color{230, 230, 30, 255}, true, true));
    }

    Node* addNode();
    Node* addNode(int x, int y);
    bool  removeNode(Node* node);
    bool  connectNodes(Node* source, Node* target, std::shared_ptr<Relation> relation);
    bool  disconnectNodes(Node* source, Node* target, std::shared_ptr<Relation> relation);

    bool      handleEvent(SDL_Event& event) override;
    void      update() override;
    void      render(SDL_Renderer* renderer) override;
    SDL_Point anchor() const noexcept override;

    Signal<Node*> onNodeLeftUp;
    void          upLeftNode(Node* node);

    Signal<Node*> onNodeLeftDown;
    void          downLeftNode(Node* node);

    Signal<int, int> onBackgroundLeftUp;
    void             backgroundLeftUp(int x, int y);

    Signal<int, int> onBackgroundLeftDown;
    void             backgroundLeftDown(int x, int y);

    Signal<Node*> onNodeConnectDown;
    void          downConnectNode(Node* node, std::shared_ptr<Relation> relation);

    bool isConnected(IWidget* source, IWidget* target) const noexcept;

    ~Canvas() { removeAnyMousePosition(); }

  private:
    TTF_Font*                 _font;
    Node*                     mp_start    = nullptr;
    Link*                     mp_link     = nullptr;
    MousePosition*            mp          = nullptr;
    std::shared_ptr<Relation> mp_relation = nullptr;

    std::vector<std::shared_ptr<Relation>> vec;

    void removeAnyMousePosition();
};
