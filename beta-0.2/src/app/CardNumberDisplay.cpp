//
// Created by apple on 2020/5/6.
//

#include <app/CardNumberDisplay.h>
#include <curses.h>
#include <string>

CardNumberDisplay::ptr CardNumberDisplay::Create(int x, int y, int number) {
    return CardNumberDisplay::ptr(new CardNumberDisplay(x, y, number));
}

CardNumberDisplay::CardNumberDisplay(int x, int y, int number)
    : UIComponent(x, y, HEIGHT + 2, WIDTH + 2), number(number) {

}

void CardNumberDisplay::Draw() {
    int ul_x = x - height / 2, ul_y = y - width / 2;

    // first line
    move(ul_x, ul_y);
    addch(ACS_ULCORNER);
    for (int i = 0; i < WIDTH; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);

    // body
    for (int i = 0; i < HEIGHT; ++i) {
        move(ul_x + 1 + i, ul_y);
        addch(ACS_VLINE);
        addstr(std::string(WIDTH, ' ').c_str());
        addch(ACS_VLINE);
    }

    // last line
    move(ul_x + HEIGHT + 1, ul_y);
    addch(ACS_LLCORNER);
    for (int i = 0; i < WIDTH; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);

    // draw number
    std::string num_str = std::to_string(number);
    if (number < 10) {
        num_str = "0" + num_str;
    }
    mvaddstr(x, y - 1, num_str.c_str());
}

void CardNumberDisplay::SetNumber(int new_number) {
    number = new_number;
}