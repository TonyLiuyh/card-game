//
// Created by apple on 2020/4/30.
//

#include "app/MainMenu.h"
#include "app/GameControl.h"
#include <curses.h>

MainMenu::ptr MainMenu::instance = nullptr;

MainMenu::ptr MainMenu::Instance() {
    if (instance == nullptr) {
        instance.reset(new MainMenu());
    }
    return instance;
}

MainMenu::MainMenu()
    : Scene(1, 5)
    , single_player_button(Button::Create("Single Player", [this] { SinglePlayer(); }, 10, 60))
    , multi_player_button(Button::Create("Multiple Player", [this] { MultiPlayer(); }, 15, 60 ))
    , help_button(Button::Create("Help", [this] { Help(); }, 20, 60))
    , version_button(Button::Create("Version", [this] { Version(); }, 25, 60))
    , exit_button(Button::Create("Exit", [this] { Exit(); }, 30, 60)) {
    canvas.AddComponent(single_player_button, 0, 0);
    canvas.AddComponent(multi_player_button, 1, 0);
    canvas.AddComponent(help_button, 2, 0);
    canvas.AddComponent(version_button, 3, 0);
    canvas.AddComponent(exit_button, 4, 0);
}

void MainMenu::SinglePlayer() {
    Close();
    GameControl::Instance()->Initialize();
    GameControl::Instance()->Open();
    GameControl::Instance()->Start();
}

void MainMenu::MultiPlayer() {
    mvaddstr(0, 0, "Coming soon");
}

void MainMenu::Help() {
    mvaddstr(0, 0, "Coming soon");
}

void MainMenu::Exit() {
    Close();
    InputHandler::Instance()->Stop();
}

void MainMenu::Version() {
    mvaddstr(0, 0, "BETA 0.1   ");
}