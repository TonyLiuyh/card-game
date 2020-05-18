//
// Created by apple on 2020/5/15.
//

#include "app/InputField.h"
#include "app/InputHandler.h"
#include <curses.h>

InputField* InputField::selected_input_field = nullptr;

bool InputField::initialized = false;

InputField::ptr InputField::Create(int x, int y, int width,
        std::function<void (const std::string &)> enter_event) {
    return ptr(new InputField(x, y, width, std::move(enter_event)));
}

void InputField::AdjustCursor() {
    if (selected_input_field != nullptr) {
        selected_input_field->SetCursor();
    }
}

InputField::InputField(int x, int y, int width, std::function<void (const std::string &)> enter_event)
    : UIComponent(x, y, 3, width + 2, true, HORIZONTAL)
    , text()
    , cursor_pos(0)
    , left_pos(0)
    , enter_event(std::move(enter_event)) {
    if (!initialized) {
        InputHandler::Identifier id = InputHandler::Instance()->NewIdentifier();
        InputHandler::Instance()->RegisterPrintableKeys(id, [](char ch) { Input(ch); });
        InputHandler::Instance()->RegisterKey(id, 127, [] { Delete(); });
        initialized = true;
    }
}

std::string InputField::Text() const {
    return text;
}

void InputField::Text(const std::string &str) {
    text = str;
    cursor_pos = text.length();
    if (cursor_pos >= width - 2) {
        left_pos = cursor_pos - width + 3;
    } else {
        left_pos = 0;
    }
}

void InputField::Input(char ch) {
    if (selected_input_field == nullptr) {
        return;
    }
    selected_input_field->EnterCharacter(ch);
}

void InputField::Delete() {
    if (selected_input_field == nullptr) {
        return;
    }
    selected_input_field->DeleteCharacter();
}

void InputField::EnterCharacter(char ch) {
    text = text.substr(0, cursor_pos) + std::string(1, ch) + text.substr(cursor_pos);
    ++cursor_pos;
    if (cursor_pos - left_pos >= width - 2) {
        ++left_pos;
    }
    Draw();
}

void InputField::DeleteCharacter() {
    if (cursor_pos == 0) {
        return;
    }
    text = text.substr(0, cursor_pos - 1) + text.substr(cursor_pos);
    --cursor_pos;
    if (cursor_pos == text.length() && left_pos > 0) {
        --left_pos;
    }
    Draw();
}

void InputField::SetCursor() {
    int start_x = x - 1, start_y = y - width / 2;
    move(start_x + 1, start_y + 1 + cursor_pos - left_pos);
}

void InputField::Draw() {
    int start_x = x - height / 2;
    int start_y = y - width / 2;
    move(start_x, start_y);

    addch(ACS_ULCORNER);
    for (int i = 0; i < width - 2; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);

    move(start_x + 1, start_y);
    addch(ACS_VLINE);
    addstr(std::string(width - 2, ' ').c_str());
    addch(ACS_VLINE);
    move(start_x + 2, start_y);
    addch(ACS_LLCORNER);
    for (int i = 0; i < width - 2; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);

    move(start_x + 1, start_y + 1);

    if (selected_input_field == this) {
        if (text.length() - left_pos > width - 2) {
            addstr(text.substr(left_pos, width - 2).c_str());
        } else {
            addstr(text.substr(left_pos).c_str());
        }
        move(start_x + 1, start_y + 1 + cursor_pos - left_pos);
    } else {
        if (text.length() > width - 2) {
            addstr(text.substr(0, width - 2).c_str());
        } else {
            addstr(text.c_str());
        }
    }
}

void InputField::Select() {
    selected_input_field = this;
    curs_set(1);
}

void InputField::Unselect() {
    selected_input_field = nullptr;
    curs_set(0);
}

void InputField::Click() {
    if (enter_event != nullptr) {
        enter_event(text);
        text.clear();
        cursor_pos = 0;
        left_pos = 0;
    }
}

bool InputField::Left() {
    if (cursor_pos == 0) {
        return false;
    }
    --cursor_pos;
    if (cursor_pos < left_pos) {
        --left_pos;
    }
    return true;
}

bool InputField::Right() {
    if (cursor_pos == text.length()) {
        return false;
    }
    ++cursor_pos;
    if (cursor_pos >= left_pos + width - 2) {
        ++left_pos;
    }
    return true;
}