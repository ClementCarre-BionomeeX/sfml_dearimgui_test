#include "../incl/widgetmanager.h"
#include "../incl/iwidget.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

void WidgetManager::handleEvents(SDL_Event& event) {
    for (auto& widget : widgets) {
        if (widget->handleEvent(event)) {
            break;    // Optional: stop processing if one widget handles the event
        }
    }
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
