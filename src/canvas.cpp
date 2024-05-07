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
                handled = true;
            }
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                backgroundLeftUp(mouseX, mouseY);
                handled = true;
            }
        }
    }

    // if (!handled) {
    switch (event.type) {
    case SDL_MOUSEWHEEL: {
        // Zoom in or out
        // Adjust zoom speed here
        float zoomIncrement = 0;
        if (event.wheel.y > 0) {      // Upward motion
            zoomIncrement = 0.1f;     // Positive increment for zooming in
        } else {                      // Downward motion
            zoomIncrement = -0.1f;    // Negative increment for zooming out
        }

        zoomFactor += zoomIncrement;
        zoomFactor = std::max(0.1f, std::min(zoomFactor, 10.0f));    // Constrain zoom factor

        break;
    }
    case SDL_MOUSEMOTION: {
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {    // Pan with middle mouse

            float adjustedXRel = static_cast<float>(event.motion.xrel) / zoomFactor;
            float adjustedYRel = static_cast<float>(event.motion.yrel) / zoomFactor;

            viewportOffset.x += static_cast<int>(adjustedXRel);
            viewportOffset.y += static_cast<int>(adjustedYRel);
        }
        break;
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
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Calculate the adjusted window dimensions based on the zoom factor
    int adjustedWidth  = static_cast<int>(static_cast<float>(windowWidth) / zoomFactor);
    int adjustedHeight = static_cast<int>(static_cast<float>(windowHeight) / zoomFactor);

    // Create an SDL_Rect to define the viewport
    SDL_Rect viewportRect = {
        viewportOffset.x,    // X coordinate
        viewportOffset.y,    // Y coordinate
        adjustedWidth,       // Width adjusted by zoom factor
        adjustedHeight       // Height adjusted by zoom factor
    };

    SDL_RenderSetScale(renderer, zoomFactor, zoomFactor);
    SDL_RenderSetViewport(renderer, &viewportRect);

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
    auto rel = std::make_shared<Relation>(
        "", SDL_Color{0, 0, 0, 255}, SDL_Color{0, 0, 0, 255}, true, true);
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
