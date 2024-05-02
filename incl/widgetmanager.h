#pragma once

#include "../incl/iwidget.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class WidgetManager {
  private:
    std::vector<std::unique_ptr<IWidget>> widgets;
    SDL_Renderer*                         renderer;

  public:
    WidgetManager(SDL_Renderer* renderer) : widgets{}, renderer{renderer} {}
    template <typename T, typename... Args>
    T*   addWidget(Args&&... args);
    void handleEvents(SDL_Event& event);
    void updateWidgets();
    void renderWidgets();
    template <typename T, typename... Args>
    T* addDraggableWidget(Args&&... args);

  private:
    // draggable managment
    IWidget* selection = nullptr;
    int      startx    = 0;
    int      starty    = 0;
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
    ptr->onMouseLeftUp.connect([&](int x, int y) { selection = nullptr; });
    ptr->onDragging.connect([&](int x, int y) { selection->moveTo(x - startx, y - starty); });
    return ptr;
}
