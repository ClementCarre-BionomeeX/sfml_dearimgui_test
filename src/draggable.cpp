#include "../incl/draggable.h"

bool Draggable::handleEvent(SDL_Event& event) {
    bool handled = false;
    int  mouseX  = event.motion.x;
    int  mouseY  = event.motion.y;

    // Handle hover state
    if (event.type == SDL_MOUSEMOTION) {
        // Update the hover state
        bool currentlyHovering = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                                  mouseY >= rect.y && mouseY <= rect.y + rect.h);
        if (currentlyHovering && !isHover) {
            onHover.emit();
            isHover = true;
        } else if (!currentlyHovering && isHover) {
            onHoverLost.emit();
            isHover = false;
        }

        // Handle dragging
        if (isSelected) {
            if (!isDragging) {
                isDragging = true;    // Start dragging
            }
            onDragging.emit(mouseX, mouseY);
            handled = true;
        }
    }

    // Handle mouse button events
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT && isHover) {
            isSelected = true;
            onMouseLeftDown.emit(mouseX, mouseY);
            handled = true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT && isSelected) {
            if (isDragging) {
                isDragging = false;    // Stop dragging
            }
            onMouseLeftUp.emit(mouseX, mouseY);
            isSelected = false;
            handled    = true;
        }
    }

    return handled;
}

void Draggable::update() {
    // Update logic for the draggable element
    // This could include updating position if being dragged, reacting to state changes, etc.
}

void Draggable::render(SDL_Renderer* renderer) {
    // Render the draggable element

    SDL_SetRenderDrawColor(
        renderer, color.r, color.g, color.b, color.a);    // Example: red color for visibility
    SDL_RenderFillRect(renderer, &rect);
}

std::pair<int, int> Draggable::anchor() const noexcept {
    // Return the center of the rectangle as an anchor point
    return {rect.x + rect.w / 2, rect.y + rect.h / 2};
}
