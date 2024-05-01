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
};

template <typename T, typename... Args>
inline T* WidgetManager::addWidget(Args&&... args) {
    auto widget = std::make_unique<T>(std::forward<Args>(args)...);
    T*   ptr    = widget.get();
    widgets.push_back(std::move(widget));
    // widgets.push_back(std::unique_ptr<IWidget>(widget.release()));
    return ptr;
}
