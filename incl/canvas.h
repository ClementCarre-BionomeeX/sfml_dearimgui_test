#pragma once

#include "../incl/iwidget.h"
#include "../incl/link.h"
#include "../incl/mouse_position.h"
#include "../incl/node.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <optional>
#include <string>

class Canvas : public IWidget, public WidgetManager {

  public:
    Canvas(non_owning_ptr<SDL_Renderer> renderer, non_owning_ptr<TTF_Font> font)
        : IWidget(), WidgetManager(renderer), _font(font), vec(),
          zoomFactor(1.0f), mp{std::make_shared<MousePosition>()},
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

    std::weak_ptr<Node> addNode(int x, int y);
    bool                removeNode(std::weak_ptr<Node> node);
    bool                connectNodes(std::weak_ptr<Node>     source,
                                     std::weak_ptr<Node>     target,
                                     std::weak_ptr<Relation> relation);
    bool                disconnectNodes(std::weak_ptr<Node>     source,
                                        std::weak_ptr<Node>     target,
                                        std::weak_ptr<Relation> relation);

    bool      handleEvent(SDL_Event& event, float zoomfactor) override;
    void      update() override;
    void      render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    SDL_Point anchor() const noexcept override;

    Signal<std::weak_ptr<Node>> onNodeLeftUp;
    Signal<std::weak_ptr<Node>> onNodeLeftDown;
    Signal<int, int>            onBackgroundLeftUp;
    Signal<int, int>            onBackgroundLeftDown;
    Signal<std::weak_ptr<Node>> onNodeConnectDown;

    void upLeftNode(std::weak_ptr<Node> node);
    void downLeftNode(std::weak_ptr<Node> node);
    void backgroundLeftUp(int x, int y);
    void backgroundLeftDown(int x, int y);
    void downConnectNode(std::weak_ptr<Node> node, std::weak_ptr<Relation> relation);

    std::optional<std::weak_ptr<Link>>
    findConnection(std::weak_ptr<IWidget>  source,
                   std::weak_ptr<IWidget>  target,
                   std::weak_ptr<Relation> relation) const noexcept;

    void disconnectAllSignals() noexcept;

    ~Canvas();

  private:
    non_owning_ptr<TTF_Font>               _font;
    std::vector<std::shared_ptr<Relation>> vec;

    float zoomFactor;    // This will track the zoom level

    std::shared_ptr<MousePosition> mp;
    std::shared_ptr<Relation>      mouse_pos_relation;
    std::weak_ptr<Node>            mp_start;
    std::unique_ptr<Link>          mp_link;
    std::weak_ptr<Relation>        mp_relation;

    std::vector<std::weak_ptr<IWidget>> widgetToRemove;

    void removeAnyMousePosition();
};
