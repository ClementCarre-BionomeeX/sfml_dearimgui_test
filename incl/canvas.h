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
    Canvas(non_owning_ptr<SDL_Renderer> renderer, non_owning_ptr<TTF_Font> font)
        : IWidget(), WidgetManager(renderer), _font(font), vec(), zoomFactor(1.0f),
          mouse_pos_relation{std::make_shared<Relation>("",
                                                        SDL_Color{0, 0, 0, 255},
                                                        SDL_Color{0, 0, 0, 255},
                                                        true,
                                                        true)} {

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

    std::shared_ptr<Node> addNode();
    std::shared_ptr<Node> addNode(int x, int y);
    bool                  removeNode(std::shared_ptr<Node> node);
    bool                  connectNodes(std::shared_ptr<Node>     source,
                                       std::shared_ptr<Node>     target,
                                       std::shared_ptr<Relation> relation);
    bool                  disconnectNodes(std::shared_ptr<Node>     source,
                                          std::shared_ptr<Node>     target,
                                          std::shared_ptr<Relation> relation);

    bool      handleEvent(SDL_Event& event, float zoomfactor) override;
    void      update() override;
    void      render(non_owning_ptr<SDL_Renderer> renderer) override;
    SDL_Point anchor() const noexcept override;

    Signal<std::shared_ptr<Node>> onNodeLeftUp;
    void                          upLeftNode(std::shared_ptr<Node> node);

    Signal<std::shared_ptr<Node>> onNodeLeftDown;
    void                          downLeftNode(std::shared_ptr<Node> node);

    Signal<int, int> onBackgroundLeftUp;
    void             backgroundLeftUp(int x, int y);

    Signal<int, int> onBackgroundLeftDown;
    void             backgroundLeftDown(int x, int y);

    Signal<std::weak_ptr<Node>> onNodeConnectDown;
    void downConnectNode(std::shared_ptr<Node> node, std::shared_ptr<Relation> relation);

    bool isConnected(std::shared_ptr<IWidget> source,
                     std::shared_ptr<IWidget> target) const noexcept;

    ~Canvas() {
        vec.clear();        // Explicitly clear the vector of shared pointers
        mp_link.reset();    // Ensure unique_ptr resources are released if applicable
        removeAnyMousePosition();
    }

  private:
    non_owning_ptr<TTF_Font> _font;

    std::shared_ptr<Node>          mp_start;
    std::unique_ptr<Link>          mp_link;
    std::shared_ptr<MousePosition> mp;

    std::shared_ptr<Relation> mp_relation = nullptr;

    std::vector<std::shared_ptr<Relation>> vec;

    float zoomFactor;    // This will track the zoom level

    void removeAnyMousePosition();

    std::shared_ptr<Relation> mouse_pos_relation;
};
