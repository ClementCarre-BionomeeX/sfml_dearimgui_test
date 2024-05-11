#include "../incl/widgetmanager.h"
#include "../incl/iwidget.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

bool WidgetManager::handleEvents(SDL_Event& event, float zoomfactor) {
    for (auto& widget : widgets) {
        if (widget->handleEvent(event, zoomfactor)) {
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
        widget->render(_renderer);
    }
}

bool WidgetManager::removeWidget(std::shared_ptr<IWidget> element) {
    // find the first reference to element
    auto to_remove = std::find_if(
        widgets.rbegin(), widgets.rend(), [element](auto const& w) { return element == w; });
    if (to_remove != widgets.rend()) {
        std::swap(*to_remove, *(widgets.end() - 1));
        widgets.erase(widgets.end() - 1);
        element.reset();
        return true;
    }
    return false;
}
