//
// Created by apple on 2020/5/14.
//

#ifndef CARD_GAME_INPUTFIELD_H
#define CARD_GAME_INPUTFIELD_H

#include "app/UIComponent.h"
#include <memory>
#include <functional>
#include <string>

class InputField : public UIComponent {
public:
    typedef std::shared_ptr<InputField> ptr;

    static ptr Create(int x, int y, int width, std::function<void(const std::string&)> enter_event = nullptr);

    static void AdjustCursor();

    void Text(const std::string& str);

    std::string Text() const;

    void Draw() override;

    void Select() override;

    void Unselect() override;

    void Click() override;

    bool Left() override;

    bool Right() override;

private:
    InputField(int x, int y, int width, std::function<void(const std::string&)> enter_event);

    static void Input(char ch);

    static void Delete();

    void EnterCharacter(char ch);

    void DeleteCharacter();

    void SetCursor();

private:
    static InputField* selected_input_field;

    static bool initialized;

    std::string text;

    int cursor_pos;

    int left_pos;

    std::function<void(const std::string&)> enter_event;
};

#endif //CARD_GAME_INPUTFIELD_H
