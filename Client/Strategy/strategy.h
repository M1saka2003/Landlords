#ifndef STRATEGY_H
#define STRATEGY_H

#include "player.h"
#include "PlayHand.h"


class Strategy {
public:
    Strategy(Player *player, const Cards &cards);

    // 1 .制定出牌策略
    Cards makeStrategy();

    // 2. 第一个出牌 firstPlay
    Cards firstPlay();

    // 3. 得到比指定牌型大的牌
    Cards getGreaterCards(PlayHand type);

    // 4. 能大过指定的牌时,判断出牌还是放行,返回true出牌,false放行
    bool whetherToBeat(Cards &cs);

    // 5. 找出指定数量(count)的相同数的牌(point),找出count张点数为point的牌
    Cards findSamePointCards(Card::CardPoint point, int count);

    // 6. 找出所有点数数量为count的牌, 得到一个多张扑克牌数组
    QVector <Cards> findCardsByCount(int count);

    // 7. 根据点数范围找牌
    Cards getRangeCards(Card::CardPoint begin, Card::CardPoint end);

    // 8. 按牌型找牌,并且指定要找的牌是否大过指定牌型
    QVector <Cards> findCardType(PlayHand hand, bool beat);

    // 9. 从指定的Cards对象中挑选出满足条件的顺子
    void pickSeqSingles(QVector <QVector<Cards>> &allSeqRecord, QVector <Cards> &seqSingle, Cards &cards);

    // 10. 最优的顺子的集合筛选函数
    QVector <Cards> pickOptimalSeqSingles();

private:

    using function = Cards (Strategy::*)(Card::CardPoint point);

    struct CardInfo {
        Card::CardPoint begin;
        Card::CardPoint end;
        int extra; // 顺子或连对的数量
        bool beat;
        int number; // 指定点数的牌的数量
        int base; // 最基础的顺子或连对的数量
        function getSeq;
    };

    QVector <Cards> getCards(Card::CardPoint point, int number);

    QVector <Cards> getTripleSingleOrPair(Card::CardPoint begin, PlayHand::HandType type);

    QVector <Cards> getPlane(Card::CardPoint begin, bool extra = false);

    QVector <Cards> getPlane2SingleOr2Pair(Card::CardPoint begin, PlayHand::HandType type);

    QVector <Cards> getBomb(Card::CardPoint begin);

    QVector <Cards> getSeqPairOrSeqSingle(CardInfo &info);

    Cards getBaseSeqPair(Card::CardPoint point);

    Cards getBaseSeqSingle(Card::CardPoint point);

private:

    Player *m_player;
    Cards m_cards;
};

#endif // STRATEGY_H
