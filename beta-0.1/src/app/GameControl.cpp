//
// Created by apple on 2020/4/27.
//

#include "app/GameControl.h"
#include "ncurses.h"
#include "app/MainMenu.h"

GameControl::ptr GameControl::instance = nullptr;

GameControl::ptr GameControl::Instance() {
    if (instance == nullptr) {
        instance.reset(new GameControl());
    }
    return instance;
}

GameControl::GameControl()
    : Scene(3, 3)
    , play_button(Button::Create("Play", [this] { Play(main_player); }, 30, 45))
    , hint_button(Button::Create("Hint", [this] { Hint(); }, 30, 60))
    , pass_button(Button::Create("Pass", [this] { Pass(); }, 30, 75))
    , x_button(Button::Create("Exit", [this] { Leave(); }, 39, 115))
    , again_button(Button::Create("Play Again", [this] { Again(); }, 25, 45))
    , leave_button(Button::Create("Leave", [this] { Leave(); }, 25, 75))
    , players()
    , card_numbers()
    , played_cards()
    , main_player(0)
    , small_cards_positions{{25, 60}, {15, 95}, {15, 25}}
    , remaining_cards_positions{{35, 12}, {8, 117}, {8, 3}} {
    canvas.AddComponent(x_button, 2, 0, 3, 1);
    canvas.AddComponent(play_button, 0, 0);
    canvas.AddComponent(hint_button, 0, 1);
    canvas.AddComponent(pass_button, 0, 2);
    canvas.AddComponent(again_button, 1, 0);
    canvas.AddComponent(leave_button, 1, 1);
}

void GameControl::Initialize() {
    for (auto & player : players) {
        canvas.RemoveComponent(player);
    }
    players.clear();
    for (auto & card : card_numbers) {
        canvas.RemoveComponent(card);
    }
    card_numbers.clear();
    for (auto & played : played_cards) {
        canvas.RemoveComponent(played);
    }
    played_cards.clear();

    x_button->SetActive(true);
    play_button->SetActive(true);
    hint_button->SetActive(true);
    pass_button->SetActive(true);
    again_button->SetActive(false);
    leave_button->SetActive(false);

    CardDeck deck;
    deck.Fill(1);
    players.push_back(CardControl::Create(
            deck.Draw(18), true,35, 60));
    players.push_back(CardControl::Create(
            deck.Draw(18), false,8, 95));
    players.push_back(CardControl::Create(
            deck.Draw(18), false,8, 25));

    canvas.AddComponent(players[0], 1, 0, 3, 1);
    for (int i = 1; i < players.size(); ++i) {
        canvas.AddComponent(players[i]);
    }
    for (int i = 0; i < players.size(); ++i) {
        card_numbers.push_back(CardNumberDisplay::Create(
                remaining_cards_positions[i][0], remaining_cards_positions[i][1],
                players[i]->RemainingCardsNumber()));

        canvas.AddComponent(card_numbers[i]);
        if (i == main_player) {
            played_cards.push_back(PlayedCardsDisplay::Create(small_cards_positions[i][0],
                    small_cards_positions[i][1], 20, 1));
        } else {
            played_cards.push_back(PlayedCardsDisplay::Create(small_cards_positions[i][0],
                    small_cards_positions[i][1], 6, 1));
        }
        canvas.AddComponent(played_cards[i]);
    }
}

void GameControl::Start() {
    int first = 0;
    for (int i = 0; i < players.size(); ++i) {
        if (players[i]->HasCard(THREE, HEART)) {
            first = i;
        }
    }

    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    played_cards[first]->Message("FIRST TO PLAY");
    played_cards[first]->Draw();
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (first != main_player) {
        Play(first);
    } else {
        played_cards[first]->Message("");
        played_cards[first]->Draw();
        play_button->SetActive(true);
        hint_button->SetActive(true);
        //pass_button->SetActive(true);
    }
}

void GameControl::Play(int player_index) {
    mvaddstr(0, 0, std::string(50, ' ').c_str());
    if (player_index != main_player) {
        players[player_index]->Hint();
    }
    CardCombination card = players[player_index]->Play();

    // this only happens for main player
    if (player_index == main_player && (card.GetType() == CardCombination::INVALID ||
            card.GetType() == CardCombination::PASS)) {
        mvaddstr(0, 0, "You cannot play in this way");
        return;
    } else if (card.GetType() == CardCombination::PASS) {
        played_cards[player_index]->Message("PASS");
        played_cards[player_index]->Draw();
    } else {
        for (int i = 0; i < players.size(); ++i) {
            if (i != player_index) {
                players[i]->LastCombination(card);
            }
        }

        played_cards[player_index]->SetCards(card.Display());
        played_cards[player_index]->Draw();
        card_numbers[player_index]->SetNumber(players[player_index]->RemainingCardsNumber());
        card_numbers[player_index]->Draw();

        if (players[player_index]->RemainingCardsNumber() == 0) {
            play_button->SetActive(false);
            hint_button->SetActive(false);
            pass_button->SetActive(false);
            for (auto & player : players) {
                player->ShowCards();
                player->Draw();
            }
            refresh();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (player_index == main_player) {
                Win();
            } else {
                Lose();
            }
            return;
        }
    }

    if (player_index == main_player) {
        play_button->SetActive(false);
        hint_button->SetActive(false);
        pass_button->SetActive(false);
    }


    int next = (player_index + 1) % players.size();
    played_cards[next]->Clear();

    if (next != main_player) {
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Play(next);
    } else {
        play_button->SetActive(true);
        hint_button->SetActive(true);
        if (!players[main_player]->IsLastCombinationPass()) {
            pass_button->SetActive(true);
        }
    }
}

void GameControl::Hint() {
    players[main_player]->Hint();
}

void GameControl::Pass() {
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    played_cards[main_player]->Message("PASS");
    played_cards[main_player]->Draw();
    players[main_player]->Pass();

    int next = (main_player + 1) % players.size();
    played_cards[next]->Clear();
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Play(next);
}

void GameControl::Again() {
    canvas.ResetSelection();
    clear();
    Initialize();
    canvas.Show();
    Start();
}

void GameControl::Leave() {
    Close();
    MainMenu::Instance()->Open();
}

void GameControl::Win() {
    DrawResult(true);
}

void GameControl::Lose() {
    DrawResult(false);
}

void GameControl::DrawResult(bool win) {
    canvas.ResetSelection();

    x_button->SetActive(false);
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    for (auto & player : players) {
        player->SetSelectable(false);
        //player->SetActive(false);
    }

    int x = 10, y = 30;
    int width = 60, height = 20;
    mvaddch(x, y, ACS_ULCORNER);
    for (int i = 0; i < width; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);

    for (int i = 0; i < height; ++i) {
        mvaddch(x + 1 + i, y, ACS_VLINE);
        addstr(std::string(width, ' ').c_str());
        addch(ACS_VLINE);
    }

    mvaddch(x + height + 1, y, ACS_LLCORNER);
    for (int i = 0; i < width; ++i) {
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);


    move(x + 5, y + 25);
    if (win) {
        addstr("You win!");
    } else {
        addstr("You lose");
    }

    again_button->SetActive(true);
    leave_button->SetActive(true);
}