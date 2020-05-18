//
// Created by apple on 2020/4/24.
//

#ifndef POKER_CARDCONTROL_H
#define POKER_CARDCONTROL_H

#include <map>
#include <set>
#include <vector>
#include <list>
#include "app/UIComponent.h"
#include "app/CardDeck.h"
#include "app/CardCombination.h"

class CardControl : public UIComponent {
public:
    typedef std::shared_ptr<CardControl> ptr;

    static const int CARD_WIDTH = 8, CARD_HEIGHT = 5;
    static const int SMALL_CARD_WIDTH = 5, SMALL_CARD_HEIGHT = 3;

    static void DrawCards(const std::vector<Card>& cards, int x, int y);

    static void DrawSmallCards(const std::vector<Card>& cards, int x, int y, int max_cards_per_line);

    static void DrawSmallCardsBack(int cards, int x, int y, int max_cards_per_line);

    static void DrawSmallCardsBackWithHeart3(const std::vector<Card>& cards, int x, int y, int max_cards_per_line);

    static int CardsWidth(int num_cards);

    virtual void Reset(const std::vector<Card>& new_cards) = 0;

    void AddCards(const std::multiset<Card>& new_cards);

    bool HasCard(CardRank rank, Suit suit);

    void Play(const std::multiset<Card>& played_cards);

    void SetMessage(const std::string& msg);

    std::multiset<Card> AutoPlay();

    // std::multiset<Card> GetCards() const;

    bool CanPlayCards() const;

    void LastCombination(const CardCombination& combination);

    bool IsLastCombinationPass() const;

    int RemainingCardsNumber() const;

protected:
    CardControl(const std::vector<Card>& cards, int x, int y, int height, int width);

    // below two methods use upper-left corner coordinates
    static void DrawCardFace(int x, int y, int height, CardRank rank, Suit suit);

    static void DrawCardBack(int x, int y, int width, int height);

protected:
    std::multiset<Card> cards;

    CardCombination last_combination;

    CardCombination last_hint;

    bool selected;

    std::string message;
};

class OpponentCardControl : public CardControl {
public:
    typedef std::shared_ptr<OpponentCardControl> ptr;

    static ptr Create(const std::vector<Card>& cards, int x, int y);

    void Draw() override;

    void Reset(const std::vector<Card>& new_cards) override;

    void ShowCards();

private:
    OpponentCardControl(const std::vector<Card>& cards, int x, int y);

private:
    bool show_cards;
};

class PlayerCardControl : public CardControl {
public:
    typedef std::shared_ptr<PlayerCardControl> ptr;

    static ptr Create(const std::vector<Card>& cards, int x, int y);

    void Reset(const std::vector<Card>& new_cards) override;

    std::multiset<Card> GetSelectedCards() const;

    void EraseSelected();

    void Hint();

    void Draw() override;

    void Select() override;

    void Unselect() override;

    bool Left() override;

    bool Right() override;

    void Click() override;

    void AltLeft() override;

    void AltRight() override;

private:
    PlayerCardControl(const std::vector<Card>& cards, int x, int y);

private:
    std::vector<bool> selected_cards;

    int pointer;

    int alt_select_flag;
};

#endif //POKER_CARDCONTROL_H
