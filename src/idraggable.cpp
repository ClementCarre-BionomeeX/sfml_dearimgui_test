#include "../incl/idraggable.h"

IDraggable::IDraggable(int x, int y, int w, int h, SDL_Color baseColor, SDL_Color hoverColor)
    : IWidget{x, y, w, h}, _baseColor{baseColor}, _hoverColor{hoverColor} {
    changeToBaseColor();

    onMouseLeftDown.connect([this](int, int) { isDragging = true; });

    onMouseLeftUp.connect([this](int, int) {
        if (isDragging) {
            isDragging = false;    // End the dragging process
        }
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

void IDraggable::drag(int x, int y, int _snapping) {
    onDragging.emit(x, y, _snapping);
}

bool IDraggable::handleEvent(SDL_Event& event, float zoomfactor) {
    bool ctrlkey = SDL_GetModState() & KMOD_CTRL;
    int  mouseX  = static_cast<int>(static_cast<float>(event.motion.x) / zoomfactor);
    int  mouseY  = static_cast<int>(static_cast<float>(event.motion.y) / zoomfactor);

    bool handled = IWidget::handleEvent(event, zoomfactor);

    // Additional handling for dragging if selected
    if (event.type == SDL_MOUSEMOTION && isDragging) {

        if (ctrlkey) {
            drag(mouseX, mouseY, static_cast<int>(20.0 / zoomfactor));    // Emit dragging signal
        } else {
            drag(mouseX, mouseY, 1);
        }

        handled = true;
    }

    return handled;
}

IDraggable::~IDraggable() {
    onDragging.disconnect_all();
};
