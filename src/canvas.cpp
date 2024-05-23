#include "../incl/canvas.h"
#include "../incl/fruchterman_reingold.h"
#include "../incl/json.h"
#include "../incl/link.h"
#include "../incl/relation.h"
#include "../incl/vecvectransform.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

std::weak_ptr<Node> Canvas::addNode(int x, int y) {

    // change value to world position
    x = static_cast<int>(static_cast<float>(x) / zoomFactor);
    y = static_cast<int>(static_cast<float>(y) / zoomFactor);

    auto ptr = addDraggableWidget<Node>(x, y, 100, 200, _font, vec);

    if (auto node = ptr.lock()) {

        node->onSelected.connect([ptr, this]() {
            for (auto link : selectedLinks) {
                if (auto locked = link.lock()) {
                    locked->unselect();
                }
            }
            selectedLinks = findAllOutboundConnections(ptr);
            for (auto link : selectedLinks) {
                if (auto locked = link.lock()) {
                    locked->select();
                }
            }

            // put the node at the top of the list
            if (auto lockedptr = ptr.lock()) {
                std::iter_swap(std::find(widgets.begin(), widgets.end(), lockedptr),
                               widgets.begin());
            }
        });

        node->onUnselected.connect([ptr, this]() {
            for (auto link : selectedLinks) {
                if (auto locked = link.lock()) {
                    locked->unselect();
                }
            }
        });

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

        node->onLabelRightDown.connect([ptr, this]() {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            if (auto sharedPtr = ptr.lock()) {
                auto modal = std::make_shared<ModalValueChanger<std::string>>(
                    mx, my, 250, 0, "Change Node Name", sharedPtr->getName(), _font);
                modal->onClosed.connect([ptr, this](std::optional<std::string> newName) {
                    if (auto sharedptr2 = ptr.lock()) {
                        if (newName.has_value()) {
                            sharedptr2->changeName(newName.value());
                        }
                    }
                    this->clearModal();
                });
                this->setModal(modal);
            }
        });

        node->onOtherRightDown.connect([ptr, this]() {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            if (auto sharedPtr = ptr.lock()) {
                auto modal = std::make_shared<ModalMenu>(
                    mx,
                    my,
                    200,
                    0,
                    _font,
                    std::vector<std::pair<std::string, Signal<>::Slot>>{
                        {
                            "Set Unknown",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    tmp_node->changeState(KnowledgeState::Unknown);
                                }
                            }    //
                        },
                        {
                            "Set Certified",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    switch (tmp_node->getState()) {
                                    case KnowledgeState::Unknown:
                                        break;
                                    case KnowledgeState::Uncertified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Certified_Acquired);
                                        break;
                                    case KnowledgeState::Uncertified_Missing:
                                        tmp_node->changeState(KnowledgeState::Certified_Missing);
                                        break;
                                    case KnowledgeState::Certified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Certified_Acquired);
                                        break;
                                    case KnowledgeState::Certified_Missing:
                                        tmp_node->changeState(KnowledgeState::Certified_Missing);
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }    //
                        },
                        {
                            "Set Uncertified",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    switch (tmp_node->getState()) {
                                    case KnowledgeState::Unknown:
                                        tmp_node->changeState(KnowledgeState::Unknown);
                                        break;
                                    case KnowledgeState::Uncertified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Acquired);
                                        break;
                                    case KnowledgeState::Uncertified_Missing:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Missing);
                                        break;
                                    case KnowledgeState::Certified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Acquired);
                                        break;
                                    case KnowledgeState::Certified_Missing:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Missing);
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }    //
                        },
                        {
                            "Set Missing",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    switch (tmp_node->getState()) {
                                    case KnowledgeState::Unknown:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Missing);
                                        break;
                                    case KnowledgeState::Uncertified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Missing);
                                        break;
                                    case KnowledgeState::Uncertified_Missing:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Missing);
                                        break;
                                    case KnowledgeState::Certified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Certified_Missing);
                                        break;
                                    case KnowledgeState::Certified_Missing:
                                        tmp_node->changeState(KnowledgeState::Certified_Missing);
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }    //
                        },
                        {
                            "Set Acquired",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    switch (tmp_node->getState()) {
                                    case KnowledgeState::Unknown:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Acquired);
                                        break;
                                    case KnowledgeState::Uncertified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Acquired);
                                        break;
                                    case KnowledgeState::Uncertified_Missing:
                                        tmp_node->changeState(KnowledgeState::Uncertified_Acquired);
                                        break;
                                    case KnowledgeState::Certified_Acquired:
                                        tmp_node->changeState(KnowledgeState::Certified_Acquired);
                                        break;
                                    case KnowledgeState::Certified_Missing:
                                        tmp_node->changeState(KnowledgeState::Certified_Acquired);
                                        break;
                                    default:
                                        break;
                                    }
                                }
                            }    //
                        },
                        {
                            "Remove Node",    //
                            [ptr, this]() {
                                if (auto tmp_node = ptr.lock()) {
                                    this->removeNode(ptr);
                                }
                            }    //
                        }        //
                    });

                modal->close.connect([this]() { killModal = true; });
                this->setModalMenu(modal);
            }
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
        widgetToRemove.push_back(link.value());
        return true;
    }
    return false;
}

bool Canvas::handleEvent(SDL_Event& event, float) {
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;

    if (activeModal) {
        return activeModal->handleEvent(event, zoomFactor);
    }
    if (activeModalMenu) {
        return activeModalMenu->handleEvent(event, zoomFactor);
    }

    bool handled = WidgetManager::handleEvents(event, zoomFactor);

    if (!handled) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                backgroundLeftDown(mouseX, mouseY);
                handled = true;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                backgroundRightDown(mouseX, mouseY);
                startDragging(mouseX, mouseY);
                handled = true;
            }

        } else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                backgroundLeftUp(mouseX, mouseY);
                handled = true;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                backgroundRightUp(mouseX, mouseY);
                endDragging(mouseX, mouseY);
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

        moveWidgetsAround({mouseX, mouseY}, oldZoomFactor);
        handled = true;

        break;
    }
    case SDL_MOUSEMOTION: {
        if (event.motion.state && isDragging) {    // Pan with middle mouse

            computeDragging(mouseX, mouseY);
            handled = true;
        }
        break;
    }
    case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_r) {
            float oldZoomFactor = zoomFactor;
            // Reset the zoom factor to 1
            zoomFactor = 1.0f;

            moveWidgetsAround({mouseX, mouseY}, oldZoomFactor);
            handled = true;
        }
        break;
    }
    }

    return handled;
}

void Canvas::startDragging(int x, int y) noexcept {
    auto nodes = find_all_by_type<Node>();
    for (auto node : nodes) {
        if (auto lockednode = node.lock()) {
            auto pos = lockednode->position();
            xstarts.push_back((int)((float)(x) / zoomFactor) - pos.x);
            ystarts.push_back((int)((float)(y) / zoomFactor) - pos.y);
        }
    }

    isDragging = true;
}
void Canvas::endDragging(int, int) noexcept {
    xstarts.clear();
    ystarts.clear();
    isDragging = false;
}
void Canvas::computeDragging(int x, int y) noexcept {
    auto        nodes = find_all_by_type<Node>();
    std::size_t i     = 0;
    for (auto node : nodes) {
        if (auto lockednode = node.lock()) {
            lockednode->moveTo(                                 //
                (int)((float)(x) / zoomFactor) - xstarts[i],    //
                (int)((float)(y) / zoomFactor) - ystarts[i]     //
            );
        }
        ++i;
    }
}

void Canvas::moveWidgetsAround(SDL_Point screenPositionTarget, float oldZoomFactor) {
    auto allwidgets = find_all_by_type<IWidget>();
    for (auto widget : allwidgets) {
        if (auto lockedwidget = widget.lock()) {

            auto worldPos = lockedwidget->position();

            int oldWorldTargetX =
                static_cast<int>(static_cast<float>(screenPositionTarget.x) / oldZoomFactor);
            int oldWorldTargetY =
                static_cast<int>(static_cast<float>(screenPositionTarget.y) / oldZoomFactor);
            int relWorldPosX = worldPos.x - oldWorldTargetX;
            int relWorldPosY = worldPos.y - oldWorldTargetY;

            int newWorldPosX =
                relWorldPosX +
                static_cast<int>(static_cast<float>(screenPositionTarget.x) / zoomFactor);
            int newWorldPosY =
                relWorldPosY +
                static_cast<int>(static_cast<float>(screenPositionTarget.y) / zoomFactor);

            lockedwidget->moveTo(newWorldPosX, newWorldPosY);
        }
    }
}

void Canvas::update() {
    // first heck if we need to load anything
    if (loadFromFile.has_value()) {
        // load json
        json j;
        try {
            std::ifstream file(loadFromFile.value());
            file >> j;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return;
        }
        // then apply
        from_json(j);
        loadFromFile.reset();
    }
    // then, remove all nodes that need to be removed
    // be sur not to emit any signals before this !
    for (auto const& widget : widgetToRemove) {
        removeWidget(widget);
    }
    widgetToRemove.clear();

    // same for modal windows
    if (killModal) {
        killModal = false;
        clearModal();
    }

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
    // then all nodes (in reverse order)
    auto nodes = find_all_by_type<Node>();
    for (auto it = std::rbegin(nodes); it != std::rend(nodes); ++it) {
        if (auto lockednode = it->lock()) {
            lockednode->render(renderer, zoomFactor);
        }
    }

    if (mp_link) {
        mp_link->render(renderer, zoomFactor);
    }

    if (activeModal) {
        activeModal->render(renderer, zoomFactor);
    }
    if (activeModalMenu) {
        activeModalMenu->render(renderer, zoomFactor);
    }
}

SDL_Point Canvas::anchor() const noexcept {
    return {0, 0};
}

void Canvas::setModal(std::shared_ptr<ModalValueChanger<std::string>> modal) {
    activeModal = modal;
}
void Canvas::setModalMenu(std::shared_ptr<ModalMenu> modal) {
    activeModalMenu = modal;
}

void Canvas::clearModal() {

    if (activeModal) {
        activeModal.reset();
    }
    if (activeModalMenu) {
        activeModalMenu.reset();
    }
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
                    return std::make_optional(link);
                }
            }
        }
    } else {
        for (auto link : links) {
            if (auto lockedlink = link.lock()) {
                if (lockedlink->isRelation(relation) &&
                    ((lockedlink->isSource(source) && lockedlink->isTarget(target)) ||
                     (lockedlink->isSource(target) && lockedlink->isTarget(source)))) {
                    return std::make_optional(link);
                }
            }
        }
    }
    return {};
}

std::vector<std::weak_ptr<Link>>
Canvas::findAllOutboundConnections(std::weak_ptr<Node> source) const noexcept {
    auto result = std::vector<std::weak_ptr<Link>>();
    auto links  = find_all_by_type<Link>();
    for (auto link : links) {
        if (auto lockedlink = link.lock()) {
            if (lockedlink->isSource(source) ||
                (!lockedlink->getRelation().lock()->directed() && lockedlink->isTarget(source))) {
                result.push_back(link);
            }
        }
    }
    return result;
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
    if (auto lockselection = selection.lock()) {
        lockselection->unselect();
    }
    onBackgroundLeftDown.emit(x, y);
}

void Canvas::backgroundRightUp(int x, int y) {
    onBackgroundRightUp.emit(x, y);
}
void Canvas::backgroundRightDown(int x, int y) {
    onBackgroundRightDown.emit(x, y);
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

json Canvas::save() const {

    json result;

    result["zoomFactor"] = zoomFactor;

    result["vec"] =
        transformVector<std::shared_ptr<Relation>, json>(vec, [](auto rel) { return rel->save(); });

    auto nodes = find_all_by_type<Node>();

    result["nodes"] =
        transformVector<std::weak_ptr<Node>, json>(nodes, [](std::weak_ptr<Node> node) {
            if (auto lockednode = node.lock()) {
                return lockednode->save();
            } else {
                return json{};
            }
        });

    auto links         = find_all_by_type<Link>();
    auto weakrelations = transformVector<std::shared_ptr<Relation>, std::weak_ptr<Relation>>(
        vec, [](auto rel) { return std::weak_ptr<Relation>(rel); });

    auto genericnodes =
        transformVector<std::weak_ptr<Node>, std::weak_ptr<IWidget>>(nodes, [](auto node) {
            if (auto lockednode = node.lock()) {
                return std::weak_ptr<IWidget>(std::static_pointer_cast<IWidget>(lockednode));
            } else {
                return std::weak_ptr<IWidget>{};
            }
        });

    result["links"] =
        transformVector<std::weak_ptr<Link>, json>(links, [genericnodes, weakrelations](auto link) {
            return link.lock()->save(genericnodes, weakrelations);
        });

    return result;
}

void Canvas::from_json(json j) {
    // first, should remove all preceding values
    removeAnyMousePosition();
    vec.clear();
    selectedLinks.clear();
    selection.reset();
    widgetToRemove.clear();
    killModal = false;

    widgets.clear();

    // do not touch mp or mp_pos_relation

    // first add the relations

    for (auto reljson : j["vec"]) {
        auto [name, basecolor, hovercolor, directed, transitive] = relationFromJson(reljson);
        vec.push_back(
            std::make_shared<Relation>(name, basecolor, hovercolor, directed, transitive));
    }

    // then read the zoomfactor
    zoomFactor = j["zoomFactor"];

    // then the nodes
    for (auto nodejson : j["nodes"]) {
        auto [name, x, y, state] = nodeFromJson(nodejson);
        auto node                = addNode(x, y);
        if (auto lockednode = node.lock()) {
            lockednode->changeName(name);
            lockednode->changeState(state);
        }
    }

    // finally the links
    for (auto linkjson : j["links"]) {
        std::shared_ptr<IWidget> shared_iwidget_source =
            widgets[linkjson["source"].get<std::size_t>()];
        std::shared_ptr<Node> shared_node_source =
            std::dynamic_pointer_cast<Node>(shared_iwidget_source);
        std::shared_ptr<IWidget> shared_iwidget_target =
            widgets[linkjson["target"].get<std::size_t>()];
        std::shared_ptr<Node> shared_node_target =
            std::dynamic_pointer_cast<Node>(shared_iwidget_target);
        if (shared_node_source && shared_node_target) {
            std::weak_ptr<Node> weak_node_source = shared_node_source;
            std::weak_ptr<Node> weak_node_target = shared_node_target;
            connectNodes(weak_node_source,
                         weak_node_target,
                         std::weak_ptr<Relation>(vec[linkjson["relation"].get<std::size_t>()])    //
            );
        }
    }
}

void Canvas::load(std::string path) {
    loadFromFile.reset();
    loadFromFile = path;
}

void Canvas::applyFruchtermanReingoldAlgorithm(non_owning_ptr<SDL_Renderer> renderer) {
    auto nodes = find_all_by_type<Node>();
    if (nodes.empty()) {
        return;
    }

    // generate a matrix object that is nnode x nnode
    Matrix mat;
    for (auto& node : nodes) {
        Vector v(nodes.size(), 0);
        // find all outbound connections
        auto        links = findAllOutboundConnections(node);
        std::size_t i     = 0;
        for (auto& othernode : nodes) {
            for (auto& link : links) {
                if (auto lockedlink = link.lock()) {
                    if ((lockedlink->isTarget(othernode)) ||    //
                        (lockedlink->isSource(othernode) && !lockedlink->isRelationDirected() &&
                         lockedlink->isTarget(node))) {
                        v[i] = 1.0;
                    }
                }
            }
            ++i;
        }
        mat.push_back(v);
    }

    auto result = fruchterman_reingold(mat);

    double fixedvalue_maybechangethis = 1000.0;

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (auto lockednode = nodes[i].lock()) {
            lockednode->moveTo(
                static_cast<int>(result[i][0] * fixedvalue_maybechangethis + 120.0 / zoomFactor),
                static_cast<int>(result[i][1] * fixedvalue_maybechangethis + 20.0 / zoomFactor));
        }
    }

    // compute center of gravity
    SDL_Point centerofgravity{0, 0};
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (auto lockednode = nodes[i].lock()) {
            auto lockednoderect = lockednode->getRect();
            centerofgravity.x += lockednoderect.x;
            centerofgravity.y += lockednoderect.y;
        }
    }

    centerofgravity.x =
        static_cast<int>(static_cast<float>(centerofgravity.x) / static_cast<float>(nodes.size()));
    centerofgravity.y =
        static_cast<int>(static_cast<float>(centerofgravity.y) / static_cast<float>(nodes.size()));

    // get middle of screen
    int w;
    int h;
    SDL_GetRendererOutputSize(renderer.get(), &w, &h);

    double middlex = static_cast<double>(w) / 2.0 / zoomFactor;
    double middley = static_cast<double>(h) / 2.0 / zoomFactor;

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (auto lockednode = nodes[i].lock()) {
            lockednode->push(static_cast<int>(middlex) - centerofgravity.x,
                             static_cast<int>(middley) - centerofgravity.y);
        }
    }
}
