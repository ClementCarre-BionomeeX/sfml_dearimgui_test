#pragma once

#include "../incl/iwidget.h"
#include "../incl/json.h"
#include "../incl/link.h"
#include "../incl/modalmenu.h"
#include "../incl/mouse_position.h"
#include "../incl/node.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include "../incl/widgetmanager.h"
#include <SDL2/SDL.h>
#include <optional>
#include <string>
using json = nlohmann::json;

class Canvas : std::enable_shared_from_this<Canvas>, public IWidget, public WidgetManager {

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
            "Is A", SDL_Color{0, 200, 0, 255}, SDL_Color{50, 250, 50, 255}, false, true));
        vec.push_back(std::make_shared<Relation>("Example Of",
                                                 SDL_Color{100, 100, 200, 255},
                                                 SDL_Color{150, 150, 250, 255},
                                                 true,
                                                 true));
        vec.push_back(std::make_shared<Relation>(
            "Explain", SDL_Color{200, 200, 0, 255}, SDL_Color{250, 250, 50, 255}, true, true));
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
    Signal<int, int>            onBackgroundRightUp;
    Signal<int, int>            onBackgroundRightDown;
    Signal<std::weak_ptr<Node>> onNodeConnectDown;

    void upLeftNode(std::weak_ptr<Node> node);
    void downLeftNode(std::weak_ptr<Node> node);
    void backgroundLeftUp(int x, int y);
    void backgroundLeftDown(int x, int y);

    void backgroundRightUp(int x, int y);
    void backgroundRightDown(int x, int y);

    void downConnectNode(std::weak_ptr<Node> node, std::weak_ptr<Relation> relation);

    std::optional<std::weak_ptr<Link>>
    findConnection(std::weak_ptr<IWidget>  source,
                   std::weak_ptr<IWidget>  target,
                   std::weak_ptr<Relation> relation) const noexcept;

    void disconnectAllSignals() noexcept;
    void moveWidgetsAround(SDL_Point screenPositionTarget, float oldZoomFactor);

    std::vector<std::weak_ptr<Link>>
    findAllOutboundConnections(std::weak_ptr<Node> source) const noexcept;

    ~Canvas();

    json save() const;
    void load(std::string path);

    void applyFruchtermanReingoldAlgorithm(non_owning_ptr<SDL_Renderer> renderer);

    float getZoomFactor() const noexcept;

  private:
    non_owning_ptr<TTF_Font>               _font;
    std::vector<std::shared_ptr<Relation>> vec;

    float zoomFactor;    // This will track the zoom level

    std::shared_ptr<MousePosition> mp;
    std::shared_ptr<Relation>      mouse_pos_relation;
    std::weak_ptr<Node>            mp_start;
    std::unique_ptr<Link>          mp_link;
    std::weak_ptr<Relation>        mp_relation;

    std::vector<std::weak_ptr<IWidget>>             widgetToRemove;
    std::shared_ptr<ModalValueChanger<std::string>> activeModal;
    std::shared_ptr<ModalMenu>                      activeModalMenu;

    void setModal(std::shared_ptr<ModalValueChanger<std::string>> modal);
    void setModalMenu(std::shared_ptr<ModalMenu> modal);
    void clearModal();
    void removeAnyMousePosition();

    bool                       killModal = false;
    std::optional<std::string> loadFromFile{};
    void                       from_json(json j);

    std::vector<std::weak_ptr<Link>> outboundsLinks;

    bool isDragging = false;

    std::vector<int> xstarts{};
    std::vector<int> ystarts{};

    void startDragging(int x, int y) noexcept;
    void endDragging(int x, int y) noexcept;
    void processDragging(int x, int y) noexcept;

    std::weak_ptr<Node>     findByName(std::string);
    std::weak_ptr<Relation> findRelByName(std::string name);
};
