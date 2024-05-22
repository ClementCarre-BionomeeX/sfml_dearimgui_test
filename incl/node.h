#pragma once

#include "../incl/button.h"
#include "../incl/idraggable.h"
#include "../incl/json.h"
#include "../incl/knowledge.h"
#include "../incl/label.h"
#include "../incl/modalmenu.h"
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
using json = nlohmann::json;

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
    Signal<>                        onTopButtonClick;
    Signal<std::string>             onNameChanged;
    Signal<>                        onGlobalMouseLeftUp;
    Signal<>                        onGlobalMouseRightDown;
    Signal<std::weak_ptr<Relation>> onConnectMouseLeftDown;
    Signal<>                        onLabelRightDown;
    Signal<>                        onOtherRightDown;

    // SLOTS
    void topButtonClick();
    void globalMouseLeftUp();
    void connectMouseLeftDown(std::weak_ptr<Relation> relation);

    void globalMouseRightDown();

    void           changeName(std::string name);
    std::string    getName() const noexcept;
    void           changeState(KnowledgeState newstate);
    KnowledgeState getState() const noexcept;

    json save() const;

  private:
    int radius;
    int margin        = 5;
    int topButtonSize = 20;

    TextButton topButton;
    Label      labelName;

    std::vector<std::unique_ptr<TextButton>> addConnectionButtonList;

    KnowledgeState state = KnowledgeState::Unknown;

    non_owning_ptr<TTF_Font> _font;
};

std::tuple<std::string, int, int, KnowledgeState> nodeFromJson(json j);
