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
#include <utility>
#include <vector>

class ModalMenu : public IWidget {
  public:
    ModalMenu(int                                                 x,
              int                                                 y,
              int                                                 w,
              int                                                 h,
              non_owning_ptr<TTF_Font>                            _font,
              std::vector<std::pair<std::string, Signal<>::Slot>> inputs)
        : IWidget{x, y, w, h}, font{_font} {

        int i = 0;
        for (auto& input : inputs) {
            buttons.emplace_back(
                std::make_shared<TextButton>(rect.x + margin,
                                             rect.y + margin + (buttonheight + margin) * i,
                                             rect.w - 2 * margin,
                                             buttonheight,
                                             SDL_Color{200, 200, 0, 255},
                                             SDL_Color{250, 250, 50, 255},
                                             2,
                                             input.first,
                                             font));
            signals.emplace_back(std::make_shared<Signal<>>());
            signals.back()->connect(input.second);
            ++i;
        }
    }

    bool handleEvent(SDL_Event& event, float) override {
        bool handled = IWidget::handleEvent(event, 1.0f);

        // not handled means clicked outside ?
        if (event.type == SDL_MOUSEBUTTONDOWN && !handled) {
            // ask for closing
            close.emit();
        }

        // now handle widgets
        for (std::size_t i = 0; i < buttons.size(); ++i) {
            if (buttons[i]->handleEvent(event, 1.0f)) {
                signals[i]->emit();
                close.emit();
            }
        }
        return handled;
    }

    void update() override {}

    void render(non_owning_ptr<SDL_Renderer> renderer, float) override {

        rect.h = margin + (buttonheight + margin) * static_cast<int>(buttons.size());

        SDL_SetRenderDrawColor(renderer.get(),
                               backgroundColor.r,
                               backgroundColor.g,
                               backgroundColor.b,
                               backgroundColor.a);
        SDL_RenderFillRect(renderer.get(), &rect);

        for (auto& button : buttons) {
            button->render(renderer, 1.0);
        }
    }

    Signal<> close;

    std::vector<std::shared_ptr<Signal<>>> signals;

  private:
    int margin       = 5;
    int buttonheight = 30;

    SDL_Color                backgroundColor = {50, 50, 50, 255};
    non_owning_ptr<TTF_Font> font;

    std::vector<std::shared_ptr<TextButton>> buttons;
};
