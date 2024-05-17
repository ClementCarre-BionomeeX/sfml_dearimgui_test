#pragma once

#include "../incl/iwidget.h"
#include "../incl/label.h"
#include "../incl/non_owning_ptr.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include "../incl/textbutton.h"
#include <SDL2/SDL.h>
#include <memory>
#include <optional>
#include <vector>

template <typename T>
class ModalValueChanger : public IWidget {
  public:
    ModalValueChanger(int                      x,
                      int                      y,
                      int                      w,
                      int                      h,
                      const std::string&       _labelText,
                      const std::string&       initialTextBoxValue,
                      non_owning_ptr<TTF_Font> _font)
        : IWidget{x, y, w, h}, labelText{_labelText}, textBoxValue{initialTextBoxValue},
          originalValue{initialTextBoxValue}, font{_font} {
        initWidgets();
    }

    bool handleEvent(SDL_Event& event, float) override {
        bool handled = IWidget::handleEvent(event, 1.0);
        for (auto& widget : widgets) {
            handled |= widget->handleEvent(event, 1.0);
        }
        return handled;
    }

    void update() override {
        for (auto& widget : widgets) {
            widget->update();
        }
    }

    void render(non_owning_ptr<SDL_Renderer> renderer, float) override {
        // Render modal background
        SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
        SDL_RenderFillRect(renderer.get(), &rect);

        // Render widgets
        for (auto& widget : widgets) {
            // As a gui element, always render at scale = 1
            widget->render(renderer, 1.0f);
        }
    }

    Signal<std::optional<T>> onClosed;

  private:
    void initWidgets() {
        // Initialize the label
        label = std::make_shared<Label>(
            rect.x + 10, rect.y + 10, rect.w - 20, 30, labelText, SDL_Color{0, 0, 0, 255}, font);
        widgets.push_back(label);

        // Initialize the text box
        textBox = std::make_shared<TextBox>(
            rect.x + 10, rect.y + 50, SDL_Color{0, 0, 0, 255}, font, rect.w - 20);
        textBox->changeText(textBoxValue);
        textBox->onTextChanged.connect(
            [this](const std::string& newText) { textBoxValue = newText; });
        widgets.push_back(textBox);

        int tbh = textBox->getHeight();

        // Initialize the buttons
        cancelButton = std::make_shared<TextButton>(rect.x + 10,
                                                    rect.y + 10 + 30 + 10 + tbh + 10,
                                                    60,
                                                    30,
                                                    SDL_Color{200, 0, 0, 255},
                                                    SDL_Color{255, 0, 0, 255},
                                                    5,
                                                    "Cancel",
                                                    font);
        cancelButton->onMouseLeftUp.connect([this](int, int) { onClosed.emit(std::nullopt); });
        widgets.push_back(cancelButton);

        validateButton = std::make_shared<TextButton>(rect.x + 80,
                                                      rect.y + 10 + 30 + 10 + tbh + 10,
                                                      60,
                                                      30,
                                                      SDL_Color{0, 200, 0, 255},
                                                      SDL_Color{0, 255, 0, 255},
                                                      5,
                                                      "Validate",
                                                      font);
        validateButton->onMouseLeftUp.connect([this](int, int) { onClosed.emit(textBoxValue); });
        widgets.push_back(validateButton);

        resetButton = std::make_shared<TextButton>(rect.x + 150,
                                                   rect.y + 10 + 30 + 10 + tbh + 10,
                                                   60,
                                                   30,
                                                   SDL_Color{0, 0, 200, 255},
                                                   SDL_Color{0, 0, 255, 255},
                                                   5,
                                                   "Reset",
                                                   font);
        resetButton->onMouseLeftUp.connect([this](int, int) {
            textBoxValue = originalValue;
            textBox->changeText(textBoxValue);
        });
        widgets.push_back(resetButton);

        rect.h = 10 + 30 + 10 + tbh + 10 + 30 + 10;
    }

    std::vector<std::shared_ptr<IWidget>> widgets;
    std::shared_ptr<Label>                label;
    std::shared_ptr<TextBox>              textBox;
    std::shared_ptr<TextButton>           cancelButton;
    std::shared_ptr<TextButton>           validateButton;
    std::shared_ptr<TextButton>           resetButton;

    std::string              labelText;
    std::string              textBoxValue;
    std::string              originalValue;
    non_owning_ptr<TTF_Font> font;
};

// #pragma once

// #include "../incl/iwidget.h"
// #include "../incl/label.h"
// #include "../incl/non_owning_ptr.h"
// #include "../incl/signal.h"
// #include "../incl/textbox.h"
// #include "../incl/textbutton.h"
// #include <SDL2/SDL.h>
// #include <memory>
// #include <optional>
// #include <string>
// #include <vector>

// template <typename T>
// class ModalValueChanger : public IWidget {
//   public:
//     ModalValueChanger(int x, int y, int w, int h) : IWidget{x, y, w, h} {}

//     bool handleEvent(SDL_Event& event, float zoomfactor) override {
//         bool handled = IWidget::handleEvent(event, zoomfactor);
//         // widgets event here

//         return handled;
//     }

//     void update() override;
//     void render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;

//     Signal<std::optional<T>> onClosed;

//   private:
//     template <typename U>
//     U convertFromString(std::string const& str) {
//         std::istringstream iss(str);
//         U                  value;
//         iss >> value;
//         return value;
//     }
// };
