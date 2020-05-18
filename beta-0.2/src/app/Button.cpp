//
// Created by apple on 2020/4/24.
//

#include "app/Button.h"
#include <curses.h>

#include <utility>

Button::ptr Button::Create(const std::string &text, std::function<void ()> callback, int x, int y) {
    return Button::ptr(new Button(text, std::move(callback), x, y));
}

Button::Button(std::string text, std::function<void ()> callback, int x, int y)
    : UIComponent(x, y, 1, text.length() + 4, true)
    , text(std::move(text))
    , callback(std::move(callback))
    , selected(false) {
}

void Button::OnClick(std::function<void ()> new_callback) {
    callback = std::move(new_callback);
}

void Button::SetText(std::string new_text) {
    text = new_text;
    width = text.length() + 4;
}

void Button::Click() {
    if (selected && active) {
        callback();
    }
}

void Button::Draw() {
    if (!active) {
        Clear();
        return;
    }
    // move cursor to upper left corner
    move(x - height / 2, y - width / 2);

    // draw differently according to whether selected
    if (selected) {
        addstr("[ ");
        addstr(text.c_str());
        addstr(" ]");
    } else {
        addstr("  ");
        addstr(text.c_str());
        addstr("  ");
    }
}

void Button::Select() {
    selected = true;
}

void Button::Unselect() {
    selected = false;
}