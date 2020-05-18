//
// Created by apple on 2020/5/12.
//

#ifndef CARD_GAME_TEXTLABEL_H
#define CARD_GAME_TEXTLABEL_H

#include "app/UIComponent.h"
#include <memory>
#include <string>
#include <list>

class TextLabel : public UIComponent {
public:
    typedef std::shared_ptr<TextLabel> ptr;

    enum HorizontalAlignment {
        LEFT,
        HORIZONTAL_MID,
        RIGHT
    };

    enum VerticalAlignment {
        TOP,
        VERTICAL_MID,
        BOT
    };

    static ptr Create(const std::string& text, int x, int y, int height, int width,
            HorizontalAlignment ha = HORIZONTAL_MID, VerticalAlignment va = VERTICAL_MID);

    void Draw() override;

    void SetText(const std::string& new_text);

    void SetText(const std::vector<std::string>& texts);

    void SetHorizontalAlignment(HorizontalAlignment new_ha);

    void SetVerticalAlignment(VerticalAlignment new_va);

private:
    TextLabel(std::string  text, int x, int y, int height, int width,
              HorizontalAlignment ha, VerticalAlignment va);

    static std::list<std::pair<std::string, int>> ResolveColorString(const std::string& str);

private:
    std::string text;

    HorizontalAlignment ha;

    VerticalAlignment va;
};

#endif //CARD_GAME_TEXTLABEL_H
