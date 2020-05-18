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
#include "app/TextLabel.h"
#include <memory>
#include "tool/Timer.h"
#include "app/Player.h"

class GameScene : public Scene {
public:
    typedef std::shared_ptr<GameScene> ptr;

    static ptr Instance();

    static void Reset();

    void Initialize(const std::vector<Player>& players, const std::vector<std::vector<Card>>& cards,
            int this_player_index, int first_player);

    void Start();

    void PlayCards(int played_index, const std::multiset<Card>& cards);

    void PlayerLeave(const boost::asio::ip::udp::endpoint& endpoint);

private:
    GameScene();

    void Play();

    void Hint();

    void Pass();

    void Again();

    void Leave();

    void OnTimer();

    void DrawResult();

    void DrawRemainingTime();

    void RemovePlayerControl();

    void StartPlaying();

    void AutoPlay();

    bool IsThisPlayerHost() const;

private:
    static GameScene::ptr instance;

    Button::ptr play_button;
    Button::ptr hint_button;
    Button::ptr pass_button;
    Button::ptr exit_button;
    Button::ptr again_button;
    Button::ptr leave_button;
    std::vector<Player> players_info;
    std::vector<TextLabel::ptr> player_info_labels;
    std::vector<CardNumberDisplay::ptr> card_numbers;
    std::vector<PlayedCardsDisplay::ptr> played_cards;

    PlayerCardControl::ptr player_card_control;
    std::vector<OpponentCardControl::ptr> opponent_card_controls;

    int this_player_index;
    int small_cards_positions[3][2];
    int remaining_cards_positions[3][2];
    // relative to this player -> 0
    int current_player_index;

    Timer current_player_timer;
    Timer start_timer;
    Timer finish_timer;
    Timer auto_play_timer;
    int remaining_seconds;
    std::unordered_map<int, int> finished_players;
};

#endif //POKER_GAMECONTROL_H
