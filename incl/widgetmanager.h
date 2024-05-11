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
    std::shared_ptr<T> addWidget(Args&&... args);
    template <typename T, typename... Args>
    std::shared_ptr<T> addDraggableWidget(Args&&... args);

    bool handleEvents(SDL_Event& event, float zoomfactor);
    void updateWidgets();
    void renderWidgets();

    bool removeWidget(std::shared_ptr<IWidget> element);

    template <typename T>
    std::vector<std::shared_ptr<T>> find_all_by_type() const noexcept;

  protected:
    // drag managment
    std::shared_ptr<IWidget>     selection;
    int                          startx = 0;
    int                          starty = 0;
    non_owning_ptr<SDL_Renderer> _renderer;

  private:
    std::vector<std::shared_ptr<IWidget>> widgets;
};

template <typename T, typename... Args>
inline std::shared_ptr<T> WidgetManager::addWidget(Args&&... args) {
    auto widget = std::make_shared<T>(std::forward<Args>(args)...);
    // auto ptr    = non_owning_ptr<T>(widget);
    widgets.push_back(widget);
    return widget;
}

template <typename T, typename... Args>
inline std::shared_ptr<T> WidgetManager::addDraggableWidget(Args&&... args) {
    auto ptr = addWidget<T>(std::forward<Args>(args)...);

    if (!ptr) {
        std::cout << "Failed to create widget." << std::endl;
        return nullptr;    // Early exit if widget creation failed
    }

    ptr->onHover.connect([ptr]() {
        if (ptr) {
            ptr->changeToHoverColor();
        }
    });
    ptr->onHoverLost.connect([ptr]() {
        if (ptr) {
            ptr->changeToBaseColor();
        }
    });
    ptr->onMouseLeftDown.connect([this, ptr](int x, int y) {
        if (!ptr) {
            return;
        }
        selection   = ptr;
        auto [a, b] = selection->position();
        startx      = x - a;
        starty      = y - b;
    });
    ptr->onMouseLeftUp.connect([this](int, int) { selection.reset(); });
    ptr->onDragging.connect([this](int x, int y) {
        if (selection) {
            selection->moveTo(x - startx, y - starty);
        }
    });

    return ptr;
}

template <typename T>
std::vector<std::shared_ptr<T>> WidgetManager::find_all_by_type() const noexcept {
    std::vector<std::shared_ptr<T>> result;
    for (auto& w : widgets) {
        if (auto t = std::dynamic_pointer_cast<T>(w)) {
            result.emplace_back(t);
        }
    }
    return result;
}
