#include "../incl/widgetmanager.h"
#include "../incl/iwidget.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

bool WidgetManager::handleEvents(SDL_Event& event) {
    for (auto& widget : widgets) {
        if (widget->handleEvent(event)) {
            return true;    // Optional: stop processing if one widget handles the event
        }
    }
    return false;
}

void WidgetManager::updateWidgets() {
    for (auto& widget : widgets) {
        widget->update();
    }
}

void WidgetManager::renderWidgets() {
    for (auto& widget : widgets) {
        widget->render(renderer);
    }
}

bool WidgetManager::removeWidget(IWidget* element) {
    // find the first reference to element
    auto to_remove = std::find_if(
        widgets.begin(), widgets.end(), [element](auto const& w) { return w.get() == element; });
    if (to_remove != widgets.end()) {
        std::swap(*to_remove, *(widgets.end() - 1));
        widgets.erase(widgets.end() - 1);
    }
    return false;
}
