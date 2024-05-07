#include "../incl/gui.h"

GUI::GUI(SDL_Window* window, int w, TTF_Font* font)
    : _window(window), rect{0, 0, w, 0},
      quitButton{10, 10, w - 20, 30, {200, 0, 0, 255}, {250, 50, 50, 255}, 2, "Quit", font},
      addNodeButon{10, 50, w - 20, 30, {200, 200, 200, 255}, {220, 220, 220, 255}, 2, "Add", font},
      saveButton{10, 90, w - 20, 30, {180, 180, 180, 255}, {220, 220, 220, 255}, 2, "Save", font},
      loadButton{10, 130, w - 20, 30, {180, 180, 180, 255}, {220, 220, 220, 255}, 2, "Load", font} {
    update();
    quitButton.onClick.connect([this]() { clickQuit(); });
    addNodeButon.onClick.connect([this]() { clickAddNode(); });
    saveButton.onClick.connect([this]() { clickSave(); });
    loadButton.onClick.connect([this]() { clickLoad(); });
}
void GUI::render(SDL_Renderer* renderer) {
    // render the background box from top to bottom
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &rect);

    // render all buttons
    quitButton.render(renderer);
    addNodeButon.render(renderer);
    saveButton.render(renderer);
    loadButton.render(renderer);
}

bool GUI::handleEvent(SDL_Event& event) {
    // first, check if any button is handled
    bool handled = quitButton.handleEvent(event) || addNodeButon.handleEvent(event) ||
                   saveButton.handleEvent(event) || loadButton.handleEvent(event);

    // check if the event happend in the background
    if (!handled) {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;

        bool is_inside = (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y &&
                          mouseY <= rect.y + rect.h);
        // handled = (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y &&
        //            mouseY <= rect.y + rect.h);

        if (is_inside) {
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    leftUpBackground();
                }
            }
            interactBackground();
        }
    }

    return handled;
}

void GUI::update() {
    // get the window size
    SDL_GetWindowSize(_window, nullptr, &rect.h);
}

void GUI::clickQuit() {
    onQuitClick.emit();
}
void GUI::clickAddNode() {
    onAddNodeClick.emit();
}
void GUI::clickSave() {
    onSaveClick.emit();
}
void GUI::clickLoad() {
    onLoadClick.emit();
}
void GUI::interactBackground() {
    onBackgroundInteraction.emit();
}
void GUI::leftUpBackground() {
    onBackgroundLeftUp.emit();
}
