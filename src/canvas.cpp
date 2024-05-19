#include "../incl/canvas.h"
#include "../incl/link.h"
#include <iostream>
#include <memory>
#include <string>

std::weak_ptr<Node> Canvas::addNode(int x, int y) {

    // change value to world position
    x = (int)((float)x / zoomFactor);
    y = (int)((float)y / zoomFactor);

    auto ptr = addDraggableWidget<Node>(x, y, 100, 200, _font, vec);

    if (auto node = ptr.lock()) {
        node->onTopButtonClick.connect([ptr, this]() {
            if (auto sharedPtr = ptr.lock()) {
                sharedPtr->disconnectAllSignals();
                removeNode(sharedPtr);
            }
        });
        node->onGlobalMouseLeftUp.connect([ptr, this]() {
            if (auto sharedPtr = ptr.lock()) {
                upLeftNode(sharedPtr);
            }
        });
        node->onConnectMouseLeftDown.connect([ptr, this](std::weak_ptr<Relation> relation) {
            if (auto sharedPtr = ptr.lock()) {
                downConnectNode(sharedPtr, relation);
            }
        });
        node->onCreateModal.connect([this](std::shared_ptr<ModalValueChanger<std::string>> modal) {
            this->setModal(modal);
        });
    }

    return ptr;
}

bool Canvas::removeNode(std::weak_ptr<Node> node) {
    // first, remove all links that have this node as source or target
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            if (lockedlink->isExtremity(node)) {
                // mark the link to be removed
                widgetToRemove.push_back(link);
            }
        }
    }
    // then mark the node to be removed
    widgetToRemove.push_back(node);
    return true;
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
        // removeWidget(link.value());
        widgetToRemove.push_back(link.value());
        return true;
    }
    return false;
}

bool Canvas::handleEvent(SDL_Event& event, float) {
    if (activeModal) {
        return activeModal->handleEvent(event, zoomFactor);
    }

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

        SDL_Keymod keyMod = SDL_GetModState();

        if (keyMod & KMOD_ALT) {
            // Special behavior when Alt key is pressed
            if (event.wheel.y < 0) {    // Zoom in
                if (zoomFactor > 1.0f) {
                    zoomFactor = 1.0f;    // If zoomFactor > 1, set to 1
                } else {
                    zoomFactor = 0.1f;    // If zoomFactor <= 1, set to minimum (0.1)
                }
            } else {    // Zoom out
                if (zoomFactor < 1.0f) {
                    zoomFactor = 1.0f;    // If zoomFactor < 1, set to 1
                } else {
                    zoomFactor = 2.0f;    // If zoomFactor >= 1, set to maximum (2.0)
                }
            }
        } else {
            // Apply zoom factor change based on mouse wheel direction
            if (event.wheel.y > 0) {    // Zoom in
                zoomFactor *= zoomSpeedFactor;
            } else {    // Zoom out
                zoomFactor /= zoomSpeedFactor;
            }

            // Clamp the zoom factor to prevent excessive zooming
            zoomFactor = std::max(0.1f, std::min(zoomFactor, 2.0f));
        }

        // Get mouse position in window coordinates
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        moveWidgetsAround({mouseX, mouseY}, oldZoomFactor);
        handled = true;

        break;
    }
    case SDL_MOUSEMOTION: {
        if (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {    // Pan with middle mouse

            int worldMouseX = (int)((float)event.motion.xrel / zoomFactor);
            int worldMouseY = (int)((float)event.motion.yrel / zoomFactor);

            auto allwidgets = find_all_by_type<IWidget>();
            for (auto widget : allwidgets) {
                if (auto lockedwidget = widget.lock()) {
                    lockedwidget->push(worldMouseX, worldMouseY);
                }
            }
            handled = true;
        }
        break;
    }
    case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_r) {
            float oldZoomFactor = zoomFactor;
            // Reset the zoom factor to 1
            zoomFactor = 1.0f;

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            moveWidgetsAround({mouseX, mouseY}, oldZoomFactor);
            handled = true;
        }
        break;
    }
    }

    return handled;
}

void Canvas::moveWidgetsAround(SDL_Point screenPositionTarget, float oldZoomFactor) {
    auto allwidgets = find_all_by_type<IWidget>();
    for (auto widget : allwidgets) {
        if (auto lockedwidget = widget.lock()) {

            auto worldPos = lockedwidget->position();

            int oldWorldTargetX = (int)((float)screenPositionTarget.x / oldZoomFactor);
            int oldWorldTargetY = (int)((float)screenPositionTarget.y / oldZoomFactor);
            int relWorldPosX    = worldPos.x - oldWorldTargetX;
            int relWorldPosY    = worldPos.y - oldWorldTargetY;

            int newWorldPosX = relWorldPosX + (int)((float)screenPositionTarget.x / zoomFactor);
            int newWorldPosY = relWorldPosY + (int)((float)screenPositionTarget.y / zoomFactor);

            lockedwidget->moveTo(newWorldPosX, newWorldPosY);
        }
    }
}

void Canvas::update() {

    // first, remove all nodes that need to be removed
    // be sur not to emit any signals before this !
    for (auto const& widget : widgetToRemove) {
        removeWidget(widget);
    }
    widgetToRemove.clear();

    updateWidgets();
    if (mp_link) {
        if (mp) {    // update only if necessary
            mp->update(zoomFactor);
        }
        mp_link->update();
    }
}

void Canvas::render(non_owning_ptr<SDL_Renderer> renderer, float) {

    // TODO have two separate lists for Links and Nodes
    // first all links
    auto links = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            lockedlink->render(renderer, zoomFactor);
        }
    }
    // then all nodes
    auto nodes = find_all_by_type<Node>();
    for (auto node : nodes) {
        if (auto lockednode = node.lock()) {
            lockednode->render(renderer, zoomFactor);
        }
    }

    if (mp_link) {
        mp_link->render(renderer, zoomFactor);
    }

    if (activeModal) {
        activeModal->render(renderer, zoomFactor);
    }
}

SDL_Point Canvas::anchor() const noexcept {
    return {0, 0};
}

void Canvas::setModal(std::shared_ptr<ModalValueChanger<std::string>> modal) {
    activeModal = modal;
}

void Canvas::clearModal() {
    activeModal.reset();
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

void Canvas::disconnectAllSignals() noexcept {
    onNodeLeftUp.disconnect_all();
    onNodeLeftDown.disconnect_all();
    onBackgroundLeftUp.disconnect_all();
    onBackgroundLeftDown.disconnect_all();
    onNodeConnectDown.disconnect_all();
}

Canvas::~Canvas() {

    disconnectAllSignals();

    vec.clear();        // Explicitly clear the vector of shared pointers
    mp_link.reset();    // Ensure unique_ptr resources are released if applicable
    removeAnyMousePosition();
}
