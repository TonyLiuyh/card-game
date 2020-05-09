#include <iostream>
#include "app/InputHandler.h"
#include "app/MainMenu.h"
#include "curses.h"

int main() {
    InputHandler::Initialize();

    // make sure that the terminal is large enough to draw everything correctly
    int x = InputHandler::ScreenSizeX(), y = InputHandler::ScreenSizeY();
    if (x < 40 || y < 120) {
        endwin();
        std::cout << "Please make sure your terminal size is at least 40 x 120\r\n";
        std::cout << "Current size: " << x << " x " << y << "\r\n";
        return 0;
    }

    InputHandler::Instance()->Start();
    MainMenu::Instance()->Open();
    InputHandler::Instance()->Run();

    return 0;
}
