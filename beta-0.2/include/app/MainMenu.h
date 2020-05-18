//
// Created by apple on 2020/4/30.
//

#ifndef POKER_MAINMENU_H
#define POKER_MAINMENU_H

#include "app/Scene.h"
#include "app/Button.h"

class MainMenu : public Scene {
public:
    typedef std::shared_ptr<MainMenu> ptr;

    static MainMenu::ptr Instance();

private:
    MainMenu();

    void SinglePlayer();

    void MultiPlayer();

    void Help();

    void Exit();

    void Version();

private:
    static ptr instance;

    Button::ptr single_player_button;
    Button::ptr multi_player_button;
    Button::ptr help_button;
    Button::ptr exit_button;
    Button::ptr version_button;
};

#endif //POKER_MAINMENU_H
