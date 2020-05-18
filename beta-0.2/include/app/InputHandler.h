//
// Created by apple on 2020/4/23.
//

#ifndef POKER_INPUTHANDLER_H
#define POKER_INPUTHANDLER_H

#include <app/CardDeck.h>
#include <unordered_map>
#include <map>
#include <thread>

#define CH_UP 3
#define CH_DOWN 2
#define CH_RIGHT 5
#define CH_LEFT 4
#define RETURN 10
#define ALT_RIGHT 102
#define ALT_LEFT 98

class InputHandler {
public:
    enum SpecialKey {
        ALT_LEFT_ARROW,
        ALT_RIGHT_ARROW
    };

    class Identifier {
    public:
        friend class InputHandler;
    private:
        explicit Identifier(int id);

        int id;
    };

    static InputHandler* Instance();

    static void Initialize();

    void Start();

    static int ScreenSizeX();
    static int ScreenSizeY();

    void Run();

    void Stop();

    Identifier NewIdentifier();

    void RegisterKey(Identifier id, SpecialKey key, std::function<void()> callback);

    void RegisterKey(Identifier id, char key, std::function<void()> callback);

    void UnregisterKey(Identifier id, SpecialKey key);

    void UnregisterKey(Identifier id, char key);

    void RegisterPrintableKeys(Identifier id, std::function<void(char ch)> callback);

    void UnregisterPrintableKeys(Identifier id);

    void RegisterAnyKey(Identifier id, std::function<void()> callback);

    void UnregisterAnyKey(Identifier id);

private:
    InputHandler();

    void MainLoop();

    void InvokeCallbacks(char key);

    void InvokeCallbacks(SpecialKey key);

    void InvokeAnyKeyCallbacks();

private:
    static InputHandler* instance;

    int next_id;

    std::unordered_map<char, std::unordered_map<int, std::function<void()>>> callbacks;

    std::unordered_map<SpecialKey, std::unordered_map<int, std::function<void()>>> special_key_callbacks;

    std::unordered_map<int, std::function<void()>> any_key_callbacks;

    std::unordered_map<int, std::function<void(char ch)>> printable_key_callbacks;

    std::thread* worker_thread;

    bool started;
};

#endif //POKER_INPUTHANDLER_H
