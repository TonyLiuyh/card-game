//
// Created by apple on 2020/4/23.
//

#include "app/InputHandler.h"
#include <curses.h>
#include <clocale>
#include <utility>
#include <vector>


#define ESC 27

InputHandler* InputHandler::instance = nullptr;

InputHandler::Identifier::Identifier(int id) : id(id) {}

InputHandler* InputHandler::Instance() {
    if (instance == nullptr) {
        instance = new InputHandler();
    }
    return instance;
}

void InputHandler::Initialize() {
    setlocale(LC_ALL, "");
    initscr();
    timeout(-1);
    curs_set(0);
    noecho();
    keypad(stdscr, true);
}

int InputHandler::ScreenSizeX() {
    int x, y;
    getmaxyx(stdscr, x, y);
    return x;
}

int InputHandler::ScreenSizeY() {
    int x, y;
    getmaxyx(stdscr, x, y);
    return y;
}

InputHandler::InputHandler()
    : next_id(0)
    , callbacks()
    , special_key_callbacks()
    , any_key_callbacks()
    , printable_key_callbacks()
    , worker_thread(nullptr)
    , started(false)
{

}

void InputHandler::Start() {
    if (!started) {
        started = true;
        worker_thread = new std::thread(&InputHandler::MainLoop, this);
    }
}

void InputHandler::Run() {
    worker_thread->join();
    delete worker_thread;
    worker_thread = nullptr;
    endwin();
}

void InputHandler::Stop() {
    started = false;
}

InputHandler::Identifier InputHandler::NewIdentifier() {
    return Identifier(next_id++);
}

void InputHandler::RegisterKey(Identifier id, SpecialKey key, std::function<void ()> callback) {
    if (special_key_callbacks.find(key) == special_key_callbacks.end()) {
        special_key_callbacks.emplace(key, std::unordered_map<int, std::function<void()>>());
    }
    special_key_callbacks[key][id.id] = std::move(callback);
}

void InputHandler::RegisterKey(Identifier id, char key, std::function<void ()> callback) {
    if (callbacks.find(key) == callbacks.end()) {
        callbacks.emplace(key, std::unordered_map<int, std::function<void()>>());
    }
    callbacks[key][id.id] = std::move(callback);
}

void InputHandler::UnregisterKey(Identifier id, char key) {
    auto iter = callbacks.find(key);
    if (iter != callbacks.end()) {
        iter->second.erase(id.id);
    }
}

void InputHandler::RegisterPrintableKeys(Identifier id, std::function<void (char)> callback) {
    printable_key_callbacks[id.id] = std::move(callback);
}

void InputHandler::UnregisterPrintableKeys(Identifier id) {
    printable_key_callbacks.erase(id.id);
}

void InputHandler::UnregisterKey(Identifier id, SpecialKey key) {
    auto iter = special_key_callbacks.find(key);
    if (iter != special_key_callbacks.end()) {
        iter->second.erase(id.id);
    }
}

void InputHandler::RegisterAnyKey(Identifier id, std::function<void()> callback) {
    any_key_callbacks[id.id] = std::move(callback);
}

void InputHandler::UnregisterAnyKey(Identifier id) {
    any_key_callbacks.erase(id.id);
}

void InputHandler::MainLoop() {
    while (started) {
        char c = getch();
        InvokeAnyKeyCallbacks();
        if (c == ESC) {
            c = getch();
            if (c == ALT_LEFT) {
                InvokeCallbacks(ALT_LEFT_ARROW);
            } else if (c == ALT_RIGHT) {
                InvokeCallbacks(ALT_RIGHT_ARROW);
            }
        } else {
            if (c >= ' ' && c <= '~') {
                for (auto & printable_key_callback : printable_key_callbacks) {
                    printable_key_callback.second(c);
                }
            }
            InvokeCallbacks(c);
        }
    }
}

void InputHandler::InvokeCallbacks(char key) {
    auto iter1 = callbacks.find(key);
    if (iter1 != callbacks.end()) {
        for (auto & iter2 : iter1->second) {
            iter2.second();
        }
    }
}

void InputHandler::InvokeCallbacks(SpecialKey key) {
    auto iter1 = special_key_callbacks.find(key);
    if (iter1 != special_key_callbacks.end()) {
        for (auto & iter2 : iter1->second) {
            iter2.second();
        }
    }
}

void InputHandler::InvokeAnyKeyCallbacks() {
    for (auto & any_key_callback : any_key_callbacks) {
        any_key_callback.second();
    }
}