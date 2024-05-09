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
        : IWidget(), WidgetManager(renderer), _font(font), vec(), viewportOffset{0, 0},
          zoomFactor(1.0f) {
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

    non_owning_ptr<Node> addNode();
    non_owning_ptr<Node> addNode(int x, int y);
    bool                 removeNode(non_owning_ptr<Node> node);
    bool                 connectNodes(non_owning_ptr<Node>      source,
                                      non_owning_ptr<Node>      target,
                                      std::shared_ptr<Relation> relation);
    bool                 disconnectNodes(non_owning_ptr<Node>      source,
                                         non_owning_ptr<Node>      target,
                                         std::shared_ptr<Relation> relation);

    bool      handleEvent(SDL_Event& event) override;
    void      update() override;
    void      render(non_owning_ptr<SDL_Renderer> renderer) override;
    SDL_Point anchor() const noexcept override;

    Signal<non_owning_ptr<Node>> onNodeLeftUp;
    void                         upLeftNode(non_owning_ptr<Node> node);

    Signal<non_owning_ptr<Node>> onNodeLeftDown;
    void                         downLeftNode(non_owning_ptr<Node> node);

    Signal<int, int> onBackgroundLeftUp;
    void             backgroundLeftUp(int x, int y);

    Signal<int, int> onBackgroundLeftDown;
    void             backgroundLeftDown(int x, int y);

    Signal<non_owning_ptr<Node>> onNodeConnectDown;
    void downConnectNode(non_owning_ptr<Node> node, std::shared_ptr<Relation> relation);

    bool isConnected(non_owning_ptr<IWidget> source, non_owning_ptr<IWidget> target) const noexcept;

    ~Canvas() { removeAnyMousePosition(); }

  private:
    TTF_Font*                      _font;
    non_owning_ptr<Node>           mp_start;
    std::unique_ptr<Link>          mp_link;
    std::unique_ptr<MousePosition> mp;
    std::shared_ptr<Relation>      mp_relation = nullptr;

    std::vector<std::shared_ptr<Relation>> vec;

    SDL_Point viewportOffset;    // This will track the top-left corner of the viewport
    float     zoomFactor;        // This will track the zoom level

    void removeAnyMousePosition();
};
