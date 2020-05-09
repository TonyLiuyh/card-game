//
// Created by apple on 2020/5/6.
//

#ifndef POKER_CARDNUMBERDISPLAY_H
#define POKER_CARDNUMBERDISPLAY_H

#include "app/UIComponent.h"
#include <memory>

class CardNumberDisplay : public UIComponent {
public:
    typedef std::shared_ptr<CardNumberDisplay> ptr;

    static ptr Create(int x = 0, int y = 0, int number = 0);

    void Draw() override;

    void SetNumber(int new_number);

private:
    static const int WIDTH = 2, HEIGHT = 1;

    CardNumberDisplay(int x, int y, int number);

    int number;
};



#endif //POKER_CARDNUMBERDISPLAY_H
