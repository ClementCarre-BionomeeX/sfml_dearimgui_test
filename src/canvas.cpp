#include "../incl/canvas.h"
#include "../incl/link.h"

Node* Canvas::addNode() {
    auto* ptr = addNode(50, 50);
    return ptr;
}

Node* Canvas::addNode(int x, int y) {
    auto* ptr = addDraggableWidget<Node>(
        x, y, 100, 200, SDL_Color{0, 200, 200, 255}, SDL_Color{30, 230, 230, 255}, font);
    ptr->onTopButtonClick.connect([&, ptr]() { removeNode(ptr); });
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
    return removeWidget(node);
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
    return WidgetManager::handleEvents(event);
    // return true;
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
