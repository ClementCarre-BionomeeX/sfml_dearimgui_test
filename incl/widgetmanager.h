#pragma once

#include "../incl/iwidget.h"
#include "../incl/node.h"
#include "../incl/non_owning_ptr.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class WidgetManager {
  public:
    WidgetManager(SDL_Renderer* renderer) : widgets{}, _renderer{renderer} {}

    template <typename T, typename... Args>
    non_owning_ptr<T> addWidget(Args&&... args);
    template <typename T, typename... Args>
    non_owning_ptr<T> addDraggableWidget(Args&&... args);

    bool handleEvents(SDL_Event& event);
    void updateWidgets();
    void renderWidgets();

    bool removeWidget(non_owning_ptr<IWidget> element);

    template <typename T>
    std::vector<non_owning_ptr<T>> find_all_by_type() const noexcept;

  protected:
    // drag managment
    non_owning_ptr<IWidget> selection;
    int                     startx = 0;
    int                     starty = 0;

  private:
    std::vector<std::unique_ptr<IWidget>> widgets;
    non_owning_ptr<SDL_Renderer>          _renderer;
};

template <typename T, typename... Args>
inline non_owning_ptr<T> WidgetManager::addWidget(Args&&... args) {
    auto widget = std::make_unique<T>(std::forward<Args>(args)...);
    auto ptr    = non_owning_ptr<T>(widget);
    widgets.push_back(std::move(widget));
    return ptr;
}

template <typename T, typename... Args>
inline non_owning_ptr<T> WidgetManager::addDraggableWidget(Args&&... args) {
    auto ptr = addWidget<T>(std::forward<Args>(args)...);

    ptr->onHover.connect([ptr]() { ptr->changeToHoverColor(); });
    ptr->onHoverLost.connect([ptr]() { ptr->changeToBaseColor(); });
    ptr->onMouseLeftDown.connect([&, ptr](int x, int y) {
        selection   = ptr;
        auto [a, b] = selection->position();
        startx      = x - a;
        starty      = y - b;
    });
    ptr->onMouseLeftUp.connect([&](int, int) { selection.clear(); });
    ptr->onDragging.connect([&](int x, int y) { selection->moveTo(x - startx, y - starty); });

    return ptr;
}

template <typename T>
std::vector<non_owning_ptr<T>> WidgetManager::find_all_by_type() const noexcept {
    std::vector<non_owning_ptr<T>> result;
    for (auto& w : widgets) {
        if (auto t = non_owning_ptr<IWidget>::dynamic_cast_to<T>(w)) {
            result.emplace_back(t);
        }
    }
    return result;
}
