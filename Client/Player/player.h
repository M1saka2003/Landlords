#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "cards.h"

class Player : public QObject {
    Q_OBJECT

public:
    enum Role {
        Lord, Farmer
    }; // 角色
    enum Sex {
        Man, Woman
    }; // 性别
    enum Direction {
        Left, Right
    }; // 头像的显示方位
    enum Type {
        Robot, User, Unknow
    }; // 玩家的类型

    explicit Player(QObject *parent = nullptr);

    explicit Player(QString name, QObject *parent = nullptr);

    // 名字
    void setName(QString name);

    QString getName();

    // 角色
    void setRole(Role role);

    Role getRole();

    // 性别
    void setSex(Sex sex);

    Sex getSex();

    // 方位
    void setDirection(Direction direction);

    Direction getDirection();

    // 玩家类型
    void setType(Type type);

    Type getType();

    // 玩家的分数
    void setScore(int score);

    int getScore();

    //游戏结果
    void SetWin(bool flag);

    bool isWin();

    // 提供当前对象的上家/下家对象
    void setPrevPlayer(Player *player);

    void setNextPlayer(Player *player);

    Player *getPrevPlayer();

    Player *getNextPlayer();

    // 叫地主/抢地主
    void grabLordBet(int point);

    // 存储扑克牌(发牌的时候得到)
    void storeDispatchCard(Card &card);

    void storeDispatchCard(Cards &cards);

    // 得到所有的牌
    Cards getCards();

    // 清空玩家手中的牌
    void clearCards();

    // 出牌
    void playHand(Cards &cards);

    // 设置出牌的玩家
    void setPendingInfo(Player *player, Cards &cards);

    Player *getPendPlayer();

    Cards getPendCards();

    // 存储出牌对象和打出的牌
    void storePendingInfo(Player *player, Cards &cards);

    virtual void prepareCallLord();

    virtual void preparePlayHand();

    virtual void thinkCallLord();

    virtual void thinkPlayHand();

signals:

    // 通知已经叫地主下注
    void notifyGrabLordBet(Player *player, int bet);

    // 通知已经出牌
    void notifyPlayHand(Player *player, Cards &cards);

protected:
    int m_score{};
    QString m_name{};
    Role m_role{};
    Sex m_sex{};
    Direction m_direction{};
    Type m_type{};
    bool m_isWin{};
    Player *m_prev{}, *m_next{};
    Cards m_cards{};
    Cards m_pendCrads{};
    Player *m_pendPlayer{};
};

#endif // PLAYER_H
