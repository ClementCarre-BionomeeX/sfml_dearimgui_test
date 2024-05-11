#include "../incl/canvas.h"
#include "../incl/link.h"
#include <iostream>
#include <memory>
#include <string>

std::weak_ptr<Node> Canvas::addNode(int x, int y) {
    auto ptr = addDraggableWidget<Node>(
        x, y, 100, 200, SDL_Color{0, 200, 200, 255}, SDL_Color{30, 230, 230, 255}, _font, vec);

    ptr.lock()->onTopButtonClick.connect([ptr, this]() { removeNode(ptr); });
    ptr.lock()->onGlobalMouseLeftUp.connect([ptr, this]() { upLeftNode(ptr); });
    ptr.lock()->onConnectMouseLeftDown.connect(
        [ptr, this](std::weak_ptr<Relation> relation) { downConnectNode(ptr, relation); });
    return ptr;
}

bool Canvas::removeNode(std::weak_ptr<Node> node) {
    // first, remove all links that have this node as source or target
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            if (lockedlink->isExtremity(node)) {
                removeWidget(link);
            }
        }
    }
    // then remove the node
    auto res = removeWidget(node);
    return res;
}

bool Canvas::connectNodes(std::weak_ptr<Node>     source,
                          std::weak_ptr<Node>     target,
                          std::weak_ptr<Relation> relation) {
    auto widget = addWidget<Link>(source, target, relation, 5);
    return widget.lock() ? true : false;
}

bool Canvas::disconnectNodes(std::weak_ptr<Node>     source,
                             std::weak_ptr<Node>     target,
                             std::weak_ptr<Relation> relation) {
    bool found = false;
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            if (lockedlink->isSource(source) && lockedlink->isTarget(target) &&
                lockedlink->isRelation(relation)) {
                removeWidget(link);
                found = true;
            }
        }
    }
    return found;
}

bool Canvas::handleEvent(SDL_Event& event, float) {

    bool handled = WidgetManager::handleEvents(event, zoomFactor);

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

    switch (event.type) {
    case SDL_MOUSEWHEEL: {

        float zoomBaseIncrement = 0.05f;
        float oldZoomFactor     = zoomFactor;
        // Zoom in or out
        // Adjust zoom speed here
        float zoomIncrement = 0;
        if (event.wheel.y > 0) {                   // Upward motion
            zoomIncrement = zoomBaseIncrement;     // Positive increment for zooming in
        } else {                                   // Downward motion
            zoomIncrement = -zoomBaseIncrement;    // Negative increment for zooming out
        }

        zoomFactor += zoomIncrement;
        zoomFactor = std::max(0.1f, std::min(zoomFactor, 10.0f));    // Constrain zoom factor

        // Get mouse position in window coordinates
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Calculate the ratio of the new zoom relative to the old zoom
        float zoomRatio = zoomFactor / oldZoomFactor;

        // Window dimensions
        int windowWidth, windowHeight;
        SDL_GetRendererOutputSize((SDL_Renderer*)_renderer, &windowWidth, &windowHeight);

        // Calculate the normalized mouse coordinates (from 0 to 1)
        float normX = (float)mouseX / (float)windowWidth;
        float normY = (float)mouseY / (float)windowHeight;

        auto allwidgets = find_all_by_type<IWidget>();
        for (auto widget : allwidgets) {
            if (auto lockedwidget = widget.lock()) {
                auto pos = lockedwidget->position();
                // Calculate the new widget position, centered around the mouse
                int newPosX = (int)((float)pos.x * zoomRatio + (float)mouseX -
                                    normX * (float)windowWidth * zoomRatio);
                int newPosY = (int)((float)pos.y * zoomRatio + (float)mouseY -
                                    normY * (float)windowHeight * zoomRatio);
                lockedwidget->moveTo(newPosX, newPosY);
            }
        }

        break;
    }
    case SDL_MOUSEMOTION: {
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {    // Pan with middle mouse

            int adjustedXRel = (int)(static_cast<float>(event.motion.xrel) / zoomFactor);
            int adjustedYRel = (int)(static_cast<float>(event.motion.yrel) / zoomFactor);

            auto allwidgets = find_all_by_type<IWidget>();
            for (auto widget : allwidgets) {
                if (auto lockedwidget = widget.lock()) {
                    lockedwidget->push(adjustedXRel, adjustedYRel);
                }
            }
        }
        break;
    }
    }

    return handled;
}

void Canvas::update() {
    updateWidgets();
    if (mp_link) {
        if (mp) {    // update only if necessary
            mp->update(zoomFactor);
        }
        mp_link->update();
    }
}

void Canvas::render(non_owning_ptr<SDL_Renderer> renderer) {
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize((SDL_Renderer*)renderer, &windowWidth, &windowHeight);
    SDL_RenderSetScale((SDL_Renderer*)renderer, zoomFactor, zoomFactor);

    // TODO have two separate lists for Links and Nodes
    // first all links
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            lockedlink->render(renderer);
        }
    }
    // then all nodes
    auto nodes = find_all_by_type<Node>();
    for (auto node : nodes) {
        if (auto lockednode = node.lock()) {
            lockednode->render(renderer);
        }
    }

    if (mp_link) {
        mp_link->render(renderer);
    }

    SDL_RenderSetScale((SDL_Renderer*)renderer, 1, 1);
}

SDL_Point Canvas::anchor() const noexcept {
    return {0, 0};
}

bool Canvas::isConnected(std::shared_ptr<IWidget> source,
                         std::shared_ptr<IWidget> target) const noexcept {
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            if (lockedlink->isSource(source) && lockedlink->isTarget(target)) {
                return true;
            }
        }
    }
    return false;
}

void Canvas::upLeftNode(std::weak_ptr<Node> node) {
    // if mp_start != node, we add a link
    if (mp_start.lock() && mp_start.lock() != node.lock()) {
        if (!disconnectNodes(mp_start, node, mp_relation.lock())) {
            connectNodes(mp_start, node, mp_relation.lock());
        }
    }
    // remove any active mouse_position if any
    removeAnyMousePosition();
    onNodeLeftUp.emit(node);
}

void Canvas::downLeftNode(std::weak_ptr<Node> node) {
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

void Canvas::downConnectNode(std::weak_ptr<Node> node, std::weak_ptr<Relation> relation) {
    // create a mouse_position widget
    mp_start = node;
    // mp       = std::make_shared<MousePosition>();
    mp_link = std::make_unique<Link>(
        node, std::weak_ptr<MousePosition>(mp), std::weak_ptr<Relation>(mouse_pos_relation), 5);
    mp_relation = relation;

    onNodeConnectDown.emit(node);
}

void Canvas::removeAnyMousePosition() {
    mp_link.reset();

    if (mp_relation.lock()) {
        mp_relation.reset();
    }
    if (mp_start.lock()) {
        mp_start.reset();
    }
}
