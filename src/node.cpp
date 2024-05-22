#include "../incl/node.h"
#include "../incl/draw_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <limits>
#include <string>

Node::Node(int                                           x,
           int                                           y,
           int                                           w,
           int                                           h,
           non_owning_ptr<TTF_Font>                      font,
           std::vector<std::shared_ptr<Relation>> const& relationList)
    : IDraggable{x, y, w, h, {0, 0, 0, 0}, {0, 0, 0, 0}},

      radius{2}, topButton(x + margin,
                           y + margin,
                           topButtonSize,
                           topButtonSize,
                           {200, 0, 0, 255},
                           {250, 50, 50, 255},
                           2,
                           "X",
                           font),
      labelName(x + margin,
                y + margin * 2 + topButtonSize,
                w - 2 * margin,
                30,
                "RMB to change",
                SDL_Color{0, 0, 0, 255},
                font),
      _font{font} {

    changeState(state);

    topButton.onClick.connect([&]() {
        topButtonClick();    //
    });

    // connect all left up and all left down to global
    onMouseLeftUp.connect([&](int, int) {
        globalMouseLeftUp();    //
    });

    topButton.onMouseLeftUp.connect([&](int, int) {
        globalMouseLeftUp();    //
    });

    labelName.onMouseLeftUp.connect([&](int, int) {
        globalMouseLeftUp();    //
    });

    labelName.onMouseRightDown.connect([this](int, int) {
        // showChangeNameModal(_font);    //
        onLabelRightDown.emit();
    });

    onMouseRightDown.connect([this](int, int) { onOtherRightDown.emit(); });
    topButton.onMouseRightDown.connect([this](int, int) { onOtherRightDown.emit(); });

    for (auto& relation : relationList) {
        addConnectionButtonList.emplace_back(std::make_unique<TextButton>(x + margin,
                                                                          y,
                                                                          w - 2 * margin,
                                                                          30,
                                                                          relation->baseColor(),
                                                                          relation->hoverColor(),
                                                                          2,
                                                                          relation->name(),
                                                                          font));
    }

    std::size_t i = 0;
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->onMouseLeftUp.connect([this](int, int) {
            globalMouseLeftUp();    //
        });
        connectionButton->onMouseRightDown.connect([this](int, int) {
            onOtherRightDown.emit();    //
        });
        connectionButton->onMouseLeftDown.connect(
            [this, i, relation = std::weak_ptr<Relation>(relationList[i])](int, int) {
                connectMouseLeftDown(relation);
            });
        ++i;
    }
}

void Node::changeState(KnowledgeState newstate) {
    state       = newstate;
    auto colors = getColor(state);

    _baseColor  = colors.first;
    _hoverColor = colors.second;
    _color      = std::make_unique<SDL_Color>(_baseColor);
}

Node::~Node() {
    disconnectAllSignals();
}

void Node::disconnectAllSignals() noexcept {
    topButton.onClick.disconnect_all();
    onMouseLeftUp.disconnect_all();
    topButton.onMouseLeftUp.disconnect_all();
    labelName.onMouseLeftUp.disconnect_all();
    labelName.onMouseRightDown.disconnect_all();
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->onMouseLeftUp.disconnect_all();
        connectionButton->onMouseLeftDown.disconnect_all();
    }
}

void Node::render(non_owning_ptr<SDL_Renderer> renderer, float zoomFactor) {
    // Scale the node's position and size by the zoomFactor
    SDL_Rect zoomedRect = {
        static_cast<int>(static_cast<float>(rect.x) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.y) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.w) * zoomFactor),    //
        static_cast<int>(static_cast<float>(rect.h) * zoomFactor)     //
    };

    int zoomedRadius = static_cast<int>(static_cast<float>(radius) * zoomFactor);

    if (isSelected) {
        SDL_SetRenderDrawColor(renderer.get(), 255, 215, 0, 255);
        roundCornerRectangle(renderer, zoomedRect, zoomedRadius);

        int border = static_cast<int>(2 * zoomFactor);
        // Draw the background
        SDL_SetRenderDrawColor(renderer.get(), _color->r, _color->g, _color->b, _color->a);
        roundCornerRectangle(renderer,
                             {
                                 zoomedRect.x + border,        //
                                 zoomedRect.y + border,        //
                                 zoomedRect.w - 2 * border,    //
                                 zoomedRect.h - 2 * border     //
                             },
                             zoomedRadius);
    } else {
        SDL_SetRenderDrawColor(renderer.get(), _color->r, _color->g, _color->b, _color->a);
        roundCornerRectangle(renderer, zoomedRect, zoomedRadius);
    }

    int i = 0;
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->render(renderer, zoomFactor);
        i++;
    }
    topButton.render(renderer, zoomFactor);
    labelName.render(renderer, zoomFactor);
}

bool Node::handleEvent(SDL_Event& event, float zoomfactor) {

    bool handled = false;

    for (auto& connectionButton : addConnectionButtonList) {
        handled |= connectionButton->handleEvent(event, zoomfactor);
    }

    if (topButton.handleEvent(event, zoomfactor) || labelName.handleEvent(event, zoomfactor) ||
        handled) {
        return true;
    }

    // call default Idraggable::handleEvent(event)
    return IDraggable::handleEvent(event, zoomfactor);
}

void Node::update() {
    rect.h = 3 * margin + topButtonSize + labelName.getRect().h +
             static_cast<int>(addConnectionButtonList.size()) * (30 + margin);

    topButton.moveTo(rect.x + margin, rect.y + margin);
    labelName.moveTo(rect.x + margin, rect.y + margin * 2 + topButtonSize);
    SDL_Rect ntb_rect = labelName.getRect();

    int i = 0;
    for (auto& connectionButton : addConnectionButtonList) {
        connectionButton->moveTo(
            rect.x + margin, rect.y + 3 * margin + topButtonSize + ntb_rect.h + i * (margin + 30));
        i++;
    }
}

void Node::topButtonClick() {
    onTopButtonClick.emit();
}

void Node::changeName(std::string str) {
    labelName.setText(str);
    onNameChanged.emit(str);
}

void Node::globalMouseLeftUp() {
    onGlobalMouseLeftUp.emit();
}

void Node::globalMouseRightDown() {
    onGlobalMouseRightDown.emit();
}

void Node::connectMouseLeftDown(std::weak_ptr<Relation> relation) {
    onConnectMouseLeftDown.emit(relation);
}
std::string Node::getName() const noexcept {
    return labelName.getText();
}

KnowledgeState Node::getState() const noexcept {
    return state;
}

json Node::save() const {
    return {
        {"name", labelName.getText()}, {"position", {rect.x, rect.y}}, {"state", to_json(state)}};
}

std::tuple<std::string, int, int, KnowledgeState> nodeFromJson(json j) {
    std::string name = j["name"];
    int         x    = j["position"][0];
    int         y    = j["position"][1];

    KnowledgeState state = from_json(j["state"]);

    return {name, x, y, state};
}
