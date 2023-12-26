#ifndef CARDS_H
#define CARDS_H

#include <QSet>
#include "card.h"

class Cards {
public:
    Cards();

    Cards(Card &card);

    enum SortType {
        Asc, Desc, NoSort
    };

    // 添加扑克牌
    void add(const Card &card);

    void add(const Cards &card);

    void add(const QVector<Cards> &cards);

    Cards &operator<<(const Card &card);

    Cards &operator<<(const Cards &cards);

    // 删除扑克牌
    void remove(const Card &card);

    void remove(const Cards &cards);

    void remove(const QVector<Cards> &cards);

    // 扑克牌的数量
    int cardCount();

    // 是否为空
    bool isEmpty();

    // 清空扑克牌
    void clear();

    // 最大点数
    Card::CardPoint maxPoint();

    // 最小点数
    Card::CardPoint minPoint();

    // 指定点数的牌的数量
    int pointCount(Card::CardPoint point);

    // 判断某张牌是否存在
    bool contains(const Card &card);

    bool contains(const Cards &cards);

    // 随机取出一张扑克牌
    Card takeRandCard();

    CardList toCardList(SortType type = Desc);

    QSet<Card> m_cards{};
};

#endif // CARDS_H
