#include "../incl/idraggable.h"

IDraggable::IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor)
    : IWidget{x, y, w, h}, _baseColor{baseColor}, _hoverColor{hoverColor} {
    changeToBaseColor();
    onMouseLeftDown.connect([this](int, int) {
        isSelected = true;    // Begin the dragging process
    });

    onMouseLeftUp.connect([this](int, int) {
        if (isDragging) {
            isDragging = false;    // End the dragging process
        }
        isSelected = false;    // Deselect the widget
    });

    onHover.connect([this]() {
        changeToHoverColor();    // Change color on hover
    });

    onHoverLost.connect([this]() {
        changeToBaseColor();    // Revert color when not hovering
    });
}

void IDraggable::changeToBaseColor() noexcept {
    _color = std::make_unique<SDL_Color>(_baseColor);
}
void IDraggable::changeToHoverColor() noexcept {
    _color = std::make_unique<SDL_Color>(_hoverColor);
}

void IDraggable::drag(int x, int y) {
    onDragging.emit(x, y);
}

bool IDraggable::handleEvent(SDL_Event& event) {
    bool handled = IWidget::handleEvent(event);

    // Additional handling for dragging if selected
    if (event.type == SDL_MOUSEMOTION && isSelected) {
        if (!isDragging) {
            isDragging = true;    // Start dragging
        }
        drag(event.motion.x, event.motion.y);    // Emit dragging signal
        handled = true;
    }

    return handled;
}
