#pragma once

#include "../incl/idraggable.h"
#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/non_owning_ptr.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class WidgetManager {
  public:
    WidgetManager(non_owning_ptr<SDL_Renderer> renderer) : _renderer{renderer}, widgets{} {}

    template <typename T, typename... Args>
    std::weak_ptr<T> addWidget(Args&&... args);
    template <typename T, typename... Args>
    std::weak_ptr<T> addDraggableWidget(Args&&... args);

    bool handleEvents(SDL_Event& event, float zoomfactor);
    void updateWidgets();
    void renderWidgets();

    bool removeWidget(std::weak_ptr<IWidget> element);

    template <typename T>
    std::vector<std::weak_ptr<T>> find_all_by_type() const noexcept;

  protected:
    // drag managment
    std::weak_ptr<IWidget>       selection;
    int                          startx = 0;
    int                          starty = 0;
    non_owning_ptr<SDL_Renderer> _renderer;

  private:
    std::vector<std::shared_ptr<IWidget>> widgets;
};

template <typename T, typename... Args>
inline std::weak_ptr<T> WidgetManager::addWidget(Args&&... args) {
    auto widget = std::make_shared<T>(std::forward<Args>(args)...);
    widgets.push_back(widget);
    return std::weak_ptr<T>(widget);
}

template <typename T, typename... Args>
inline std::weak_ptr<T> WidgetManager::addDraggableWidget(Args&&... args) {
    auto ptr = addWidget<T>(std::forward<Args>(args)...);

    if (auto draggable = ptr.lock()) {

        draggable->onHover.connect([ptr]() {
            if (auto sharedPtr = ptr.lock()) {
                sharedPtr->changeToHoverColor();
            }
        });

        draggable->onHoverLost.connect([ptr]() {
            if (auto sharedPtr = ptr.lock()) {
                sharedPtr->changeToBaseColor();
            }
        });

        draggable->onMouseLeftDown.connect([this, ptr](int x, int y) {
            if (auto sharedPtr = ptr.lock()) {
                selection   = ptr;
                auto [a, b] = sharedPtr->position();
                startx      = x - a;
                starty      = y - b;
            }
        });

        draggable->onMouseLeftUp.connect([this](int, int) {
            if (auto lockedSelection = selection.lock()) {
                lockedSelection.reset();
            }
        });

        draggable->onDragging.connect([this](int x, int y) {
            if (auto lockedSelection = selection.lock()) {
                lockedSelection->moveTo(x - startx, y - starty);
            }
        });
    }

    return ptr;
}

template <typename T>
std::vector<std::weak_ptr<T>> WidgetManager::find_all_by_type() const noexcept {
    std::vector<std::weak_ptr<T>> result;
    for (auto& w : widgets) {
        if (auto t = std::dynamic_pointer_cast<T>(w)) {
            result.emplace_back(std::weak_ptr<T>(t));
        }
    }
    return result;
}
