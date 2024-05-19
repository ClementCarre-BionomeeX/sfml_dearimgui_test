#pragma once

#include "../incl/button.h"
#include "../incl/idraggable.h"
#include "../incl/knowledge.h"
#include "../incl/label.h"
#include "../incl/modalvaluechanger.h"
#include "../incl/relation.h"
#include "../incl/signal.h"
#include "../incl/textbox.h"
#include "../incl/textbutton.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Node : public IDraggable {
  public:
    Node(int                                           x,
         int                                           y,
         int                                           w,
         int                                           h,
         non_owning_ptr<TTF_Font>                      font,
         std::vector<std::shared_ptr<Relation>> const& relationList);

    ~Node();

    void render(non_owning_ptr<SDL_Renderer> renderer, float zoomfactor) override;
    bool handleEvent(SDL_Event& event, float zoomfactor) override;
    void update() override;

    void disconnectAllSignals() noexcept;

    // SIGNALS
    Signal<>                                                onTopButtonClick;
    Signal<std::string>                                     onNameChanged;
    Signal<>                                                onGlobalMouseLeftUp;
    Signal<std::weak_ptr<Relation>>                         onConnectMouseLeftDown;
    Signal<std::shared_ptr<ModalValueChanger<std::string>>> onCreateModal;

    // SLOTS
    void topButtonClick();
    void changeName(std::string name);
    void globalMouseLeftUp();
    void connectMouseLeftDown(std::weak_ptr<Relation> relation);

    void changeState(KnowledgeState newstate);

  private:
    int radius;
    int margin        = 5;
    int topButtonSize = 20;

    TextButton topButton;
    Label      labelName;

    std::vector<std::unique_ptr<TextButton>>        addConnectionButtonList;
    std::shared_ptr<ModalValueChanger<std::string>> nameChangeModal;
    non_owning_ptr<TTF_Font>                        _font;

    KnowledgeState state = KnowledgeState::Unknown;
    void           showChangeNameModal(non_owning_ptr<TTF_Font> font);
};
