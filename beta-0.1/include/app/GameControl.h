//
// Created by apple on 2020/4/27.
//

#ifndef POKER_GAMECONTROL_H
#define POKER_GAMECONTROL_H

#include "app/Canvas.h"
#include "app/Button.h"
#include "app/CardControl.h"
#include "app/Scene.h"
#include "app/CardNumberDisplay.h"
#include "app/PlayedCardsDisplay.h"
#include <memory>

class GameControl : public Scene {
public:
    typedef std::shared_ptr<GameControl> ptr;

    static ptr Instance();

    void Initialize();

    void Start();

private:
    GameControl();

    void Play(int player_index);

    void Hint();

    void Pass();

    void Again();

    void Leave();

    void Win();

    void Lose();

    void DrawResult(bool win);

private:
    static GameControl::ptr instance;

    Button::ptr play_button;
    Button::ptr hint_button;
    Button::ptr pass_button;
    Button::ptr x_button;
    Button::ptr again_button;
    Button::ptr leave_button;
    std::vector<CardControl::ptr> players;
    std::vector<CardNumberDisplay::ptr> card_numbers;
    std::vector<PlayedCardsDisplay::ptr> played_cards;

    int main_player;
    int small_cards_positions[3][2];
    int remaining_cards_positions[3][2];
};

#endif //POKER_GAMECONTROL_H
