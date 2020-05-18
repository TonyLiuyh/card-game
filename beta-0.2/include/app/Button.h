//
// Created by apple on 2020/4/24.
//

#ifndef POKER_BUTTON_H
#define POKER_BUTTON_H

#include <string>
#include <memory>
#include "app/Canvas.h"

// Button UI component class
class Button : public UIComponent {
public:
    typedef std::shared_ptr<Button> ptr;

    // (x, y) is the center coordinates
    static Button::ptr Create(const std::string& text,
            std::function<void()> callback = nullptr, int x = 0, int y = 0);

    // set onclick event
    void OnClick(std::function<void()> callback);

    void SetText(std::string text);

    // override methods for UI component
    void Draw() override;
    void Select() override;
    void Unselect() override;
    void Click() override;

private:
    // private constructor
    Button(std::string  text, std::function<void()> callback, int x, int y);

    // text shown on the button
    std::string text;

    // onclick event
    std::function<void()> callback;

    // whether this button is selected
    bool selected;

};

#endif //POKER_BUTTON_H
