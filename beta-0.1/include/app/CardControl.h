//
// Created by apple on 2020/4/24.
//

#ifndef POKER_CARDCONTROL_H
#define POKER_CARDCONTROL_H

#include <map>
#include <set>
#include <vector>
#include "app/UIComponent.h"
#include "app/CardDeck.h"
#include "app/CardCombination.h"

class CardControl : public UIComponent {
public:
    static const int CARD_WIDTH = 8, CARD_HEIGHT = 5;
    static const int SMALL_CARD_WIDTH = 5, SMALL_CARD_HEIGHT = 3;

    typedef std::shared_ptr<CardControl> ptr;

    static ptr Create(const std::multiset<Card>& initial_cards, bool main_player, int x = 0, int y = 0);

    static void DrawCards(const std::vector<Card>& cards, int x, int y);

    static void DrawSmallCards(const std::vector<Card>& cards, int x, int y, int max_cards_per_line);

    static void DrawSmallCardsBack(int cards, int x, int y, int max_cards_per_line);

    static void DrawSmallCardsBackWithHeart3(const std::vector<Card>& cards, int x, int y, int max_cards_per_line);

    static int CardsWidth(int num_cards);

    void AddCards(const std::multiset<Card>& new_cards);

    bool HasCard(CardRank rank, Suit suit);

    CardCombination Play();

    void Pass();

    void Hint();

    void LastCombination(const CardCombination& combination);

    bool IsLastCombinationPass() const;

    int RemainingCardsNumber() const;

    void ShowCards();

    void Draw() override;

    void Select() override;

    void Unselect() override;

    bool Left() override;

    bool Right() override;

    void Click() override;

    void AltLeft() override;

    void AltRight() override;

private:
    // below two methods use upper-left corner coordinates
    static void DrawCardFace(int x, int y, int height, CardRank rank, Suit suit);

    static void DrawCardBack(int x, int y, int width, int height);

    explicit CardControl(const std::multiset<Card>& initial_cards, bool main_player, int x, int y);

private:
    std::multimap<Card, bool> cards;

    std::multimap<Card, bool>::reverse_iterator current_card;

    CardCombination last_combination;

    int pointer;

    int select_flag;

    bool selected;

    bool main_player;

    bool show_cards;
};

#endif //POKER_CARDCONTROL_H
