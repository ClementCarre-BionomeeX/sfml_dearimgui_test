#include "../incl/gui.h"

GUI::GUI(non_owning_ptr<SDL_Window> window, int w, non_owning_ptr<TTF_Font> font)
    : _window(window), rect{0, 0, w, 0},
      quitButton{10, 10, w - 20, 30, {200, 0, 0, 255}, {250, 50, 50, 255}, 2, "Quit", font},
      addNodeButon{10, 50, w - 20, 30, {200, 200, 200, 255}, {220, 220, 220, 255}, 2, "Add", font},
      saveButton{10, 90, w - 20, 30, {180, 180, 180, 255}, {220, 220, 220, 255}, 2, "Save", font},
      loadButton{10, 130, w - 20, 30, {180, 180, 180, 255}, {220, 220, 220, 255}, 2, "Load", font},
      fruchtermanButton{10,
                        170,
                        w - 20,
                        30,
                        {180, 180, 180, 255},
                        {220, 220, 220, 255},
                        2,
                        "F-R algorithm",
                        font} {
    update();
    quitButton.onClick.connect([this]() { clickQuit(); });
    addNodeButon.onClick.connect([this]() { clickAddNode(); });
    saveButton.onClick.connect([this]() { clickSave(); });
    loadButton.onClick.connect([this]() { clickLoad(); });
    fruchtermanButton.onClick.connect([this]() { clickFruchterman(); });
}
void GUI::render(non_owning_ptr<SDL_Renderer> renderer) {
    // render the background box from top to bottom
    SDL_SetRenderDrawColor(renderer.get(), 150, 150, 150, 255);
    SDL_RenderFillRect(renderer.get(), &rect);

    // render all buttons
    quitButton.render(renderer, 1.f);
    addNodeButon.render(renderer, 1.f);
    saveButton.render(renderer, 1.f);
    loadButton.render(renderer, 1.f);
    fruchtermanButton.render(renderer, 1.f);
}

bool GUI::handleEvent(SDL_Event& event, float zoomfactor) {
    // first, check if any button is handled
    bool handled =
        quitButton.handleEvent(event, zoomfactor) || addNodeButon.handleEvent(event, zoomfactor) ||
        saveButton.handleEvent(event, zoomfactor) || loadButton.handleEvent(event, zoomfactor) ||
        fruchtermanButton.handleEvent(event, zoomfactor);

    // check if the event happend in the background
    if (!handled) {

        int mouseX = static_cast<int>(static_cast<float>(event.motion.x) / zoomfactor);
        int mouseY = static_cast<int>(static_cast<float>(event.motion.y) / zoomfactor);

        bool is_inside = (mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y &&
                          mouseY <= rect.y + rect.h);

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
    SDL_GetWindowSize((SDL_Window*)_window, nullptr, &rect.h);
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
void GUI::clickFruchterman() {
    onFruchtermanClick.emit();
}

GUI::~GUI() {
    onQuitClick.disconnect_all();
    onAddNodeClick.disconnect_all();
    onSaveClick.disconnect_all();
    onLoadClick.disconnect_all();
    onBackgroundInteraction.disconnect_all();
    onBackgroundLeftUp.disconnect_all();
}
