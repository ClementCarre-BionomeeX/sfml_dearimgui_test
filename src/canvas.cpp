#include "../incl/canvas.h"
#include "../incl/link.h"
#include <iostream>
#include <memory>
#include <string>

Node* Canvas::addNode() {
    auto* ptr = addNode(50, 50);
    return ptr;
}

Node* Canvas::addNode(int x, int y) {

    auto* ptr = addDraggableWidget<Node>(
        x, y, 100, 200, SDL_Color{0, 200, 200, 255}, SDL_Color{30, 230, 230, 255}, _font, vec);

    ptr->onTopButtonClick.connect([&, ptr]() { removeNode(ptr); });

    ptr->onGlobalMouseLeftUp.connect([&, ptr]() { upLeftNode(ptr); });

    ptr->onConnectMouseLeftDown.connect(
        [&, ptr](std::shared_ptr<Relation> relation) { downConnectNode(ptr, relation); });
    return ptr;
}

bool Canvas::removeNode(Node* node) {
    // first, remove all links that have this node as source or target
    auto links = find_all_by_type<Link>();
    for (auto* link : links) {
        if (link->isExtremity(node)) {
            removeWidget(link);
        }
    }
    // then remove the node
    auto res = removeWidget(node);
    return res;
}

bool Canvas::connectNodes(Node* source, Node* target, std::shared_ptr<Relation> relation) {
    addWidget<Link>(source, target, relation, 5);
    return false;
}

bool Canvas::disconnectNodes(Node* source, Node* target, std::shared_ptr<Relation> relation) {
    bool found = false;
    auto links = find_all_by_type<Link>();
    for (auto* link : links) {
        if (link->isSource(source) && link->isTarget(target) && link->isRelation(relation)) {
            removeWidget(link);
            found = true;
        }
    }
    return found;
}

bool Canvas::handleEvent(SDL_Event& event) {
    bool handled = WidgetManager::handleEvents(event);
    if (!handled) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                backgroundLeftDown(mouseX, mouseY);
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                backgroundLeftUp(mouseX, mouseY);
            }
        }
    }

    return handled;
}

void Canvas::update() {
    updateWidgets();
    if (mp_link) {
        mp_link->update();
    }
}

void Canvas::render(SDL_Renderer* renderer) {
    // TODO have two separate lists for Links and Nodes

    // first all links
    auto links = find_all_by_type<Link>();
    for (auto* link : links) {
        link->render(renderer);
    }
    // then all nodes
    auto nodes = find_all_by_type<Node>();
    for (auto* node : nodes) {
        node->render(renderer);
    }

    // WidgetManager::renderWidgets();
    if (mp_link) {
        mp_link->render(renderer);
    }
}

SDL_Point Canvas::anchor() const noexcept {
    return {0, 0};
}

bool Canvas::isConnected(IWidget* source, IWidget* target) const noexcept {
    auto links = find_all_by_type<Link>();
    for (auto* link : links) {
        if (link->isSource(source) && link->isTarget(target)) {
            return true;
        }
    }
    return false;
}

void Canvas::upLeftNode(Node* node) {
    // if mp_start != node, we add a link
    if (mp_start && mp_start != node) {
        if (!disconnectNodes(mp_start, node, mp_relation)) {
            connectNodes(mp_start, node, mp_relation);
        }
    }
    // remove any active mouse_position if any
    removeAnyMousePosition();
    onNodeLeftUp.emit(node);
}

void Canvas::downLeftNode(Node* node) {
    onNodeLeftDown.emit(node);
}

void Canvas::backgroundLeftUp(int x, int y) {
    // remove any active mouse_position if any
    removeAnyMousePosition();
    onBackgroundLeftUp.emit(x, y);
}

void Canvas::backgroundLeftDown(int x, int y) {
    onBackgroundLeftDown.emit(x, y);
}

void Canvas::downConnectNode(Node* node, std::shared_ptr<Relation> relation) {
    // create a mouse_position widget
    auto rel    = std::make_shared<Relation>("", SDL_Color{0, 0, 0, 255}, SDL_Color{0, 0, 0, 255});
    mp_start    = node;
    mp          = new MousePosition();
    mp_link     = new Link(node, mp, rel, 5);
    mp_relation = relation;
    onNodeConnectDown.emit(node);
    // mp_link->debug.connect([&](std::string str) { std::cout << str << std::endl; });
}

void Canvas::removeAnyMousePosition() {
    if (mp_link) {
        delete mp_link;
        mp_link = nullptr;
    }
    if (mp) {
        delete mp;
        mp = nullptr;
    }
    if (mp_relation) {
        mp_relation = nullptr;
    }
    if (mp_start) {
        mp_start = nullptr;
    }
}
