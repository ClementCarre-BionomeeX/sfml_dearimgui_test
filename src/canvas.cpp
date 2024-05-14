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
    auto link = findConnection(source, target, relation);
    if (link) {
        removeWidget(link.value());
        return true;
    }
    return false;
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
        float zoomSpeedFactor = 1.1f;    // Adjust zoom factor speed
        float oldZoomFactor   = zoomFactor;

        // Apply zoom factor change based on mouse wheel direction
        if (event.wheel.y > 0) {    // Zoom in
            zoomFactor *= zoomSpeedFactor;
        } else {    // Zoom out
            zoomFactor /= zoomSpeedFactor;
        }

        // Clamp the zoom factor to prevent excessive zooming
        zoomFactor = std::max(0.1f, std::min(zoomFactor, 2.0f));

        // Get mouse position in window coordinates
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        float zoomRatio = zoomFactor / oldZoomFactor;

        auto allwidgets = find_all_by_type<IWidget>();
        for (auto widget : allwidgets) {
            if (auto lockedwidget = widget.lock()) {
                auto pos = lockedwidget->position();

                // Calculate the relative distance from each widget to the mouse cursor before
                // zooming
                int relPosX = pos.x - mouseX;
                int relPosY = pos.y - mouseY;

                // Apply the zoom ratio to the relative positions
                int scaledRelPosX = (int)((float)relPosX * zoomRatio);
                int scaledRelPosY = (int)((float)relPosY * zoomRatio);

                // Set the new position of each widget to keep the relative distance the same
                int newPosX = mouseX + scaledRelPosX;
                int newPosY = mouseY + scaledRelPosY;

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

    SDL_RenderSetScale((SDL_Renderer*)renderer, 1.0f, 1.0f);
}

SDL_Point Canvas::anchor() const noexcept {
    return {0, 0};
}

std::optional<std::weak_ptr<Link>>
Canvas::findConnection(std::weak_ptr<IWidget>  source,
                       std::weak_ptr<IWidget>  target,
                       std::weak_ptr<Relation> relation) const noexcept {
    auto links = find_all_by_type<Link>();
    // if relation is not directed, source or target make no sens (lol)
    if (relation.lock()->directed()) {
        for (auto link : links) {
            if (auto lockedlink = link.lock()) {
                if (lockedlink->isRelation(relation) && lockedlink->isSource(source) &&
                    lockedlink->isTarget(target)) {
                    return link;
                }
            }
        }
    } else {
        for (auto link : links) {
            if (auto lockedlink = link.lock()) {
                if (lockedlink->isRelation(relation) &&
                    ((lockedlink->isSource(source) && lockedlink->isTarget(target)) ||
                     (lockedlink->isSource(target) && lockedlink->isTarget(source)))) {
                    return link;
                }
            }
        }
    }
    return {};
}

void Canvas::upLeftNode(std::weak_ptr<Node> node) {
    // if mp_start != node, we add a link
    if (mp_start.lock() && mp_start.lock() != node.lock()) {
        if (!disconnectNodes(mp_start, node, mp_relation)) {
            connectNodes(mp_start, node, mp_relation);
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
    mp_link  = std::make_unique<Link>(
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
