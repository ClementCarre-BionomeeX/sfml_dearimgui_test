#include "../incl/canvas.h"
#include "../incl/link.h"
#include <iostream>
#include <string>

Node* Canvas::addNode() {
    auto* ptr = addNode(50, 50);
    return ptr;
}

Node* Canvas::addNode(int x, int y) {
    auto* ptr = addDraggableWidget<Node>(
        x, y, 100, 200, SDL_Color{0, 200, 200, 255}, SDL_Color{30, 230, 230, 255}, font);
    ptr->onTopButtonClick.connect([&, ptr]() { removeNode(ptr); });

    ptr->onGlobalMouseLeftUp.connect([&, ptr]() { upLeftNode(ptr); });
    ptr->onConnectMouseLeftDown.connect([&, ptr]() { downConnectNode(ptr); });

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

bool Canvas::connectNodes(Node* source, Node* target) {
    addWidget<Link>(source, target, SDL_Color{0, 200, 200, 255}, SDL_Color{30, 230, 230, 255}, 5);
    return false;
}

bool Canvas::disconnectNodes(Node* source, Node* target) {
    bool found = false;
    auto links = find_all_by_type<Link>();
    for (auto* link : links) {
        if (link->isSource(source) && link->isTarget(target)) {
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
}

void Canvas::render(SDL_Renderer* renderer) {
    // render background

    WidgetManager::renderWidgets();
}

std::pair<int, int> Canvas::anchor() const noexcept {
    return {0, 0};
}
