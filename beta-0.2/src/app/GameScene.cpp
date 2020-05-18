//
// Created by apple on 2020/4/27.
//

#include "app/GameScene.h"
#include "ncurses.h"
#include "app/MainMenu.h"
#include "app/AppManager.h"
#include "app/MultiplayerScene.h"

GameScene::ptr GameScene::instance = nullptr;

GameScene::ptr GameScene::Instance() {
    if (instance == nullptr) {
        instance.reset(new GameScene());
    }
    return instance;
}

void GameScene::Reset() {
    instance.reset();
}

GameScene::GameScene()
    : Scene(3, 3)
    , play_button(Button::Create("Play", [this] { Play(); }, 30, 45))
    , hint_button(Button::Create("Hint", [this] { Hint(); }, 30, 60))
    , pass_button(Button::Create("Pass", [this] { Pass(); }, 30, 75))
    , exit_button(Button::Create("Exit", [this] { Leave(); }, 39, 115))
    , again_button(Button::Create("Play Again", [this] { Again(); }, 25, 45))
    , leave_button(Button::Create("Leave", [this] { Leave(); }, 25, 75))
    , players_info()
    , player_info_labels(3)
    , player_card_control(PlayerCardControl::Create(std::vector<Card>(), 35, 60))
    , opponent_card_controls(2)
    , card_numbers(3)
    , played_cards(3)
    , this_player_index(0)
    , small_cards_positions{{25, 60}, {15, 95}, {15, 25}}
    , remaining_cards_positions{{35, 12}, {8, 117}, {8, 3}}
    , current_player_index(0)
    , current_player_timer(AppManager::Instance()->GetIoService(), 1000, [this]{ OnTimer(); }, -1)
    , start_timer(AppManager::Instance()->GetIoService(), 3000, [this]{ StartPlaying(); })
    , finish_timer(AppManager::Instance()->GetIoService(), 3000, [this] { DrawResult(); })
    , auto_play_timer(AppManager::Instance()->GetIoService(), 1000, [this] { AutoPlay(); })
    , remaining_seconds(0)
    , finished_players() {
    canvas.AddComponent(exit_button, 2, 0, 3, 1);
    canvas.AddComponent(play_button, 0, 0);
    canvas.AddComponent(hint_button, 0, 1);
    canvas.AddComponent(pass_button, 0, 2);
    canvas.AddComponent(again_button, 1, 0);
    canvas.AddComponent(leave_button, 1, 1);
    canvas.AddComponent(player_card_control, 1, 0, 3, 1);
    opponent_card_controls[0] = OpponentCardControl::Create(std::vector<Card>(), 6, 90);
    opponent_card_controls[1] = OpponentCardControl::Create(std::vector<Card>(), 6, 30);

    for (auto & opponent_card_control : opponent_card_controls) {
        canvas.AddComponent(opponent_card_control);
    }

    card_numbers[0] = CardNumberDisplay::Create(35, 12);
    card_numbers[1] = CardNumberDisplay::Create(8, 117);
    card_numbers[2] = CardNumberDisplay::Create(8, 3);

    played_cards[0] = PlayedCardsDisplay::Create(25, 60, 20, 1);
    played_cards[1] = PlayedCardsDisplay::Create(15, 95, 6, 2);
    played_cards[2] = PlayedCardsDisplay::Create(15, 25, 6, 2);

    player_info_labels[0] = TextLabel::Create("", 30, 12, 2, 10);
    player_info_labels[1] = TextLabel::Create("", 3, 115, 2, 10);
    player_info_labels[2] = TextLabel::Create("", 3, 5, 2, 10);

    for (int i = 0; i < card_numbers.size(); ++i) {
        canvas.AddComponent(card_numbers[i]);
        canvas.AddComponent(played_cards[i]);
        canvas.AddComponent(player_info_labels[i]);
    }
}

void GameScene::Initialize(const std::vector<Player>& players,
        const std::vector<std::vector<Card>>& cards, int player_index, int first_player) {
    finished_players.clear();
    players_info = players;
    for (int i = 0; i < players.size(); ++i) {
        player_info_labels[i]->SetText(players[(player_index + i) % 3].name);
    }

    this_player_index = player_index;
    current_player_index = (first_player - this_player_index + 3) % 3;
    player_card_control->Reset(cards[player_index]);
    for (int i = 0; i < opponent_card_controls.size(); ++i) {
        opponent_card_controls[i]->Reset(cards[(player_index + i + 1) % cards.size()]);
    }

    card_numbers[0]->SetNumber(player_card_control->RemainingCardsNumber());
    for (int i = 1; i < card_numbers.size(); ++i) {
        card_numbers[i]->SetNumber(opponent_card_controls[i - 1]->RemainingCardsNumber());
    }

    for (auto & played_card : played_cards) {
        played_card->Message("");
    }

    played_cards[(first_player - player_index + 3) % 3]->Message("FIRST TO PLAY");


    player_card_control->SetSelectable(false);
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    again_button->SetActive(false);
    leave_button->SetActive(false);
    exit_button->SetActive(true);
}

void GameScene::Start() {
    start_timer.Start();
    remaining_seconds = 20;
}

void GameScene::PlayCards(int played_index, const std::multiset<Card> &cards) {
    CardCombination combination(cards);
    int opponent_index = (played_index - this_player_index + 3) % 3;
    CardControl::ptr player_to_play;
    if (opponent_index == 0) { // this player plays the cards
        player_card_control->EraseSelected();
        player_to_play = player_card_control;
        player_to_play->Play(cards);
        // tell opponents what this player played
        for (auto & opponent_card_control : opponent_card_controls) {
            opponent_card_control->LastCombination(combination);
        }
    } else { // an opponent plays the cards
        player_to_play = opponent_card_controls[opponent_index - 1];
        player_to_play->Play(cards);
        // tell this player and other opponents what this opponent played
        player_card_control->LastCombination(combination);
        for (int i = 0; i < opponent_card_controls.size(); ++i) {
            if (i != opponent_index - 1) {
                opponent_card_controls[i]->LastCombination(combination);
            }
        }
    }

    if (cards.empty()) {
        played_cards[opponent_index]->Message("PASS");
        played_cards[opponent_index]->Draw();
    } else {
        player_to_play->Draw();
        card_numbers[opponent_index]->SetNumber(player_to_play->RemainingCardsNumber());
        card_numbers[opponent_index]->Draw();
        played_cards[opponent_index]->SetCards(combination.Display());
        played_cards[opponent_index]->Draw();

        if (player_to_play->RemainingCardsNumber() == 0) {
            if (finished_players.empty()) {
                player_to_play->SetMessage("FIRST PLACE");
                player_to_play->Draw();
                finished_players.emplace(opponent_index, 0);
            } else if (finished_players.size() == 1) {
                player_to_play->SetMessage("SECOND PLACE");
                player_to_play->Draw();
                finished_players.emplace(opponent_index, 1);
                for (int i = 0; i < 3; ++i) {
                    if (finished_players.find(i) == finished_players.end()) {
                        if (i != 0) {
                            opponent_card_controls[i - 1]->ShowCards();
                            opponent_card_controls[i - 1]->Draw();
                        }
                        finished_players.emplace(i, 2);
                        break;
                    }
                }
                refresh();
                finish_timer.Start();
                current_player_timer.Stop();
                return;
            }
        }
    }

    current_player_timer.Restart();
    remaining_seconds = 20;

    current_player_index = (current_player_index + 1) % 3;
    while (finished_players.find(current_player_index) != finished_players.end()) {
        played_cards[current_player_index]->Message("");
        played_cards[current_player_index]->Draw();
        current_player_index = (current_player_index + 1) % 3;
    }

    if (current_player_index == 0) {
        player_card_control->SetSelectable(true);
        if (player_card_control->CanPlayCards()) {
            play_button->SetActive(true);
            hint_button->SetActive(true);
            pass_button->SetActive(!player_card_control->IsLastCombinationPass());
        } else {
            play_button->SetActive(false);
            hint_button->SetActive(false);
            pass_button->SetActive(true);
        }
        play_button->Draw();
        hint_button->Draw();
        pass_button->Draw();
    }

    DrawRemainingTime();

    if (IsThisPlayerHost() && players_info[(this_player_index + current_player_index) % 3].is_robot) {
        auto_play_timer.Start();
    }
}

void GameScene::PlayerLeave(const boost::asio::ip::udp::endpoint& endpoint) {
    int player_index = 0;
    for (int i = 0; i < players_info.size(); ++i) {
        if (players_info[i].endpoint == endpoint) {
            players_info[i].is_robot = true;
            player_index = i;
        }
    }

    if (IsThisPlayerHost() && (player_index - this_player_index + 3) % 3 == current_player_index) {
        auto_play_timer.Start();
    }
}

void GameScene::Play() {
    mvaddstr(0, 0, std::string(50, ' ').c_str());

    std::multiset<Card> cards = player_card_control->GetSelectedCards();

    if (cards.empty()) {
        return;
    } else {
        CardCombination combination(cards);
        if (combination.GetType() == CardCombination::INVALID) {
            mvaddstr(0, 0, "You cannot play in this way");
            return;
        }

        RemovePlayerControl();

        AppManager::Instance()->PlayCards(cards, this_player_index);

        refresh();
    }
}

void GameScene::Hint() {
    player_card_control->Hint();
    player_card_control->Draw();
}

void GameScene::Pass() {
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    player_card_control->SetSelectable(false);
    player_card_control->EraseSelected();

    current_player_timer.Stop();

    play_button->Draw();
    hint_button->Draw();
    pass_button->Draw();
    player_card_control->Draw();

    played_cards[current_player_index]->Message("");
    played_cards[current_player_index]->Draw();

    AppManager::Instance()->PlayCards(std::multiset<Card>(), this_player_index);
    refresh();
}

void GameScene::Again() {
    AppManager::Instance()->FinishGame(true);
}

void GameScene::Leave() {
    start_timer.Stop();
    auto_play_timer.Stop();
    current_player_timer.Stop();
    finish_timer.Stop();
    AppManager::Instance()->FinishGame(false);
}

void GameScene::DrawResult() {
    canvas.ResetSelection();

    exit_button->SetActive(false);
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);

    player_card_control->SetSelectable(false);

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

    mvaddstr(x + 2, y + 27, "RESULT");

    for (int i = 0; i < 3; ++i) {
        move(x + 5 + finished_players[i] * 2, y + 15);
        std::string str = std::to_string(finished_players[i] + 1) + ". " +
                players_info[(this_player_index + i) % 3].name;
        addstr(str.c_str());
    }

    again_button->SetActive(true);
    leave_button->SetActive(true);

    again_button->Draw();
    leave_button->Draw();

    refresh();
}

void GameScene::OnTimer() {
    if (remaining_seconds > 0) {
        --remaining_seconds;
        if (remaining_seconds == 0) {
            if (current_player_index == 0) {
                RemovePlayerControl();
                refresh();
            }

            if (IsThisPlayerHost()) {
                std::multiset<Card> cards;
                if (current_player_index == 0) {
                    if (player_card_control->IsLastCombinationPass()) {
                        cards = player_card_control->AutoPlay();
                    }
                } else if (opponent_card_controls[current_player_index - 1]->IsLastCombinationPass()) {
                    cards = opponent_card_controls[current_player_index - 1]->AutoPlay();
                }

                AppManager::Instance()->PlayCards(cards, (this_player_index + current_player_index) % 3);
            }
        } else {
            DrawRemainingTime();
        }
    }
}

void GameScene::DrawRemainingTime() {
    std::string str("⧖ ");
    if (remaining_seconds < 10) {
        str += "0";
    }
    str += std::to_string(remaining_seconds);
    played_cards[current_player_index]->Message(str);
    played_cards[current_player_index]->Draw();
    refresh();
}

void GameScene::RemovePlayerControl() {
    play_button->SetActive(false);
    hint_button->SetActive(false);
    pass_button->SetActive(false);
    player_card_control->SetSelectable(false);
    player_card_control->EraseSelected();

    current_player_timer.Stop();

    play_button->Draw();
    hint_button->Draw();
    pass_button->Draw();
    player_card_control->Draw();

    played_cards[current_player_index]->Message("");
    played_cards[current_player_index]->Draw();
}

void GameScene::StartPlaying() {
    current_player_timer.Start();
    if (this_player_index == 0 && players_info[current_player_index].is_robot) {
        auto_play_timer.Start();
    }
    if (current_player_index == 0) {
        play_button->SetActive(true);
        hint_button->SetActive(true);
        play_button->Draw();
        hint_button->Draw();
        player_card_control->SetSelectable(true);
    }
    DrawRemainingTime();
}

void GameScene::AutoPlay() {
    AppManager::Instance()->PlayCards(
            opponent_card_controls[current_player_index - 1]->AutoPlay(),
            (this_player_index + current_player_index) % 3);
}

bool GameScene::IsThisPlayerHost() const {
    for (int i = 0; i < players_info.size(); ++i) {
        if (!players_info[i].is_robot && i != this_player_index) {
            return false;
        } else if (i == this_player_index) {
            return true;
        }
    }
    return true;
}