#pragma once

#include "../incl/iwidget.h"
#include "../incl/node.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class WidgetManager {
  public:
    WidgetManager(SDL_Renderer* renderer) : widgets{}, _renderer{renderer} {}

    template <typename T, typename... Args>
    T* addWidget(Args&&... args);
    template <typename T, typename... Args>
    T* addDraggableWidget(Args&&... args);

    bool handleEvents(SDL_Event& event);
    void updateWidgets();
    void renderWidgets();

    bool removeWidget(IWidget* element);

    template <typename T>
    std::vector<T*> find_all_by_type() const noexcept;

  protected:
    // drag managment
    IWidget* selection = nullptr;
    int      startx    = 0;
    int      starty    = 0;

  private:
    std::vector<std::unique_ptr<IWidget>> widgets;
    SDL_Renderer*                         _renderer;
};

template <typename T, typename... Args>
inline T* WidgetManager::addWidget(Args&&... args) {
    auto widget = std::make_unique<T>(std::forward<Args>(args)...);
    T*   ptr    = widget.get();
    widgets.push_back(std::move(widget));
    return ptr;
}

template <typename T, typename... Args>
inline T* WidgetManager::addDraggableWidget(Args&&... args) {
    auto* ptr = addWidget<T>(std::forward<Args>(args)...);
    ptr->onHover.connect([ptr]() { ptr->changeToHoverColor(); });
    ptr->onHoverLost.connect([ptr]() { ptr->changeToBaseColor(); });
    ptr->onMouseLeftDown.connect([&, ptr](int x, int y) {
        selection   = ptr;
        auto [a, b] = selection->position();
        startx      = x - a;
        starty      = y - b;
    });
    ptr->onMouseLeftUp.connect([&](int, int) { selection = nullptr; });
    ptr->onDragging.connect([&](int x, int y) { selection->moveTo(x - startx, y - starty); });
    return ptr;
}

template <typename T>
std::vector<T*> WidgetManager::find_all_by_type() const noexcept {
    std::vector<T*> result;
    for (auto& w : widgets) {
        if (T* t = dynamic_cast<T*>(w.get())) {
            result.push_back(t);
        }
    }
    return result;
}
