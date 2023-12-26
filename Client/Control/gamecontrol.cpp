#include "playhand.h"
#include "gamecontrol.h"
#include <QRandomGenerator>
#include <QTimer>
#include <qDebug>

GameControl::GameControl(QObject *parent)
        : QObject{parent} {}

void GameControl::playerInit() {
    m_robotLeft = new Robot("机器人A", this);
    m_robotRight = new Robot("机器人B", this);
    m_user = new UserPlayer("我自己Me", this);

    // 头像的显示
    m_robotLeft->setDirection(Player::Left);
    m_robotRight->setDirection(Player::Right);
    m_user->setDirection(Player::Right);

    // 性别
    Player::Sex sex;
    sex = (Player::Sex) QRandomGenerator::global()->bounded(2);
    m_robotLeft->setSex(sex);
    sex = (Player::Sex) QRandomGenerator::global()->bounded(2);
    m_robotRight->setSex(sex);
    sex = (Player::Sex) QRandomGenerator::global()->bounded(2);
    m_user->setSex(sex);

    // 出牌顺序
    m_user->setPrevPlayer(m_robotLeft);
    m_user->setNextPlayer(m_robotRight);

    m_robotLeft->setPrevPlayer(m_robotRight);
    m_robotLeft->setNextPlayer(m_user);

    m_robotRight->setPrevPlayer(m_user);
    m_robotRight->setNextPlayer(m_robotLeft);

    // 指定当前玩家

    m_currPlayer = m_user;

    // 处理玩家发射的信号
    connect(m_user, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotLeft, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotRight, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);

    // 传递出牌玩家对象和玩家打出的牌
    connect(this, &GameControl::pendingInfo, m_robotLeft, &Robot::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_robotRight, &Robot::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_user, &Robot::storePendingInfo);

    // 处理玩家出牌
    connect(m_robotLeft, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
    connect(m_robotRight, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
    connect(m_user, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
}

Robot *GameControl::getLeftRobot() {
    return m_robotLeft;
}

Robot *GameControl::getRightRobot() {
    return m_robotRight;
}

UserPlayer *GameControl::getUserPlayer() {
    return m_user;
}

void GameControl::setCurrentPlayer(Player *player) {
    m_currPlayer = player;
}

Player *GameControl::getCurrentPlayer() {
    return m_currPlayer;
}

Player *GameControl::getPendPlayer() {
    return m_pendPlayer;
}

Cards GameControl::getPendCards() {
    return m_pendCards;
}

void GameControl::setPendPlayer(Player *player) {
    m_pendPlayer = player;
}

void GameControl::initAllCards() {
    m_allCards.clear();
    for (int p = Card::Card_Begin + 1; p < Card::Card_SJ; ++p) {
        for (int s = Card::Suit_Begin + 1; s < Card::Suit_End; ++s) {
            Card c((Card::CardPoint) p, (Card::CardSuit) s);
            m_allCards.add(c);
        }
    }
    m_allCards.add(Card(Card::Card_SJ, Card::Suit_Begin));
    m_allCards.add(Card(Card::Card_BJ, Card::Suit_Begin));

}

Card GameControl::takeOneCard() {
    return m_allCards.takeRandCard();
}

Cards GameControl::getSurplusCards() {
    return m_allCards;
}

void GameControl::resetCardData() {
    // 洗牌
    initAllCards();
    // 清空所有玩家的牌
    m_robotLeft->clearCards();
    m_robotRight->clearCards();
    m_user->clearCards();
    // 初始化出牌玩家和牌
    m_pendPlayer = nullptr;
    m_pendCards.clear();
}

void GameControl::startLordCard() {
    m_currPlayer->prepareCallLord();
    emit playerStatusChanged(m_currPlayer, ThinkingForCallLord); // GamePanel::onPlayerStatusChanged
}

void GameControl::becomeLord(Player *player, int bet) {
    m_curBet = bet;
    player->setRole(Player::Lord);
    player->getPrevPlayer()->setRole(Player::Farmer);
    player->getNextPlayer()->setRole(Player::Farmer);

    m_currPlayer = player;

    player->storeDispatchCard(m_allCards);

    QTimer::singleShot(1000, this, [=] {
        emit gameStatusChanged(PlayingHand); // GamePanel::gameStatusPress
        emit playerStatusChanged(player, ThinkingForPlayHand); // GamePanel::onPlayerStatusChanged
        m_currPlayer->preparePlayHand();
    });
}

void GameControl::clearPlayerScore() {
    m_robotLeft->setScore(0);
    m_robotRight->setScore(0);
    m_user->setScore(0);
}

int GameControl::getPlayerMaxBet() {
    return m_betRecord.bet;
}

void GameControl::onGrabBet(Player *player, int bet) {
    // 1.通知主界面玩家叫地主(更新信息提示)
    if (bet == 0 && m_betRecord.bet == 0) {
        emit notifyGrabLordBet(player, 0, true);
    }
    else if (bet == 0 || m_betRecord.bet >= bet) {
        // 根据bet信息决定显示哪些按钮
        emit notifyGrabLordBet(player, 0, false); // GamePanel::onGrabLordBet
    }
    else if (bet > 0 && m_betRecord.bet == 0) {
        // 第一个抢地主的人
        emit notifyGrabLordBet(player, bet, true);
    }
    else {
        // 第二,三个抢地主的玩家
        emit notifyGrabLordBet(player, bet, false);

    }
    // 2.判断玩家下注是不是三分,如果是抢地主结束
    if (bet == 3) {
        // 玩家成为地主
        becomeLord(player, 3);
        // 清空数据
        m_betRecord.reset();
        return;
    }
    // 3.下注不够三分,对玩家的分数进行比较,分数高的是地主
    if (m_betRecord.bet < bet) {
        m_betRecord.bet = bet;
        m_betRecord.player = player;
    }
    m_betRecord.times++;
    // 如果每个玩家都抢过一次地主,抢地主结束
    if (m_betRecord.times == 3) {
        if (m_betRecord.bet == 0) {
            // 这个信号绑定到 GamePanel::gameStatusPress 槽函数
            emit gameStatusChanged(DispatchCard);
        }
        else {
            becomeLord(m_betRecord.player, m_betRecord.bet);
        }
        m_betRecord.reset();
        return;
    }
    // 4.切换玩家通知下一个玩家继续抢地主
    m_currPlayer = player->getNextPlayer();
    // 发送信号给主页面,告知当前状态为抢地主,接收这个信号后,因为是抢地主状态,如果plyaer是玩家则显示抢地主的状态栏
    // 这个信号绑定到 GamePanel::onPlayerStatusChanged 槽函数
    emit playerStatusChanged(m_currPlayer, ThinkingForCallLord);
    m_currPlayer->prepareCallLord();
}

void GameControl::onPlayHand(Player *player, Cards &cards) {
    // 1. 将玩家出牌的信号转发给主界面
    emit notifyPlayHand(player, cards);
    // 2. 如果不是空牌给其它玩家发送信号, 保存出牌玩家和打出的牌
    if (!cards.isEmpty()) {
        m_pendCards = cards;
        m_pendPlayer = player;
        emit pendingInfo(player, cards);
    }
    // 炸弹低分翻倍
    PlayHand::HandType type = PlayHand(cards).getHandType();
    if (type == PlayHand::Hand_Bomb || type == PlayHand::Hand_Bomb_Jokers) {
        m_curBet *= 2;
    }
    // 3. 如果玩家的牌出完了, 计算出本局游戏的总分
    if (player->getCards().isEmpty()) {
        Player *prev = player->getPrevPlayer();
        Player *next = player->getNextPlayer();
        if (player->getRole() == Player::Lord) {
            player->setScore(player->getScore() + 2 * m_curBet);
            prev->setScore(prev->getScore() - m_curBet);
            next->setScore(next->getScore() - m_curBet);
            player->SetWin(true), prev->SetWin(false), next->SetWin(false);
        }
        else {
            player->SetWin(true);
            player->setScore(player->getScore() + m_curBet);
            if (prev->getRole() == Player::Lord) {
                prev->setScore(prev->getScore() - 2 * m_curBet);
                next->setScore(next->getScore() + m_curBet);
                prev->SetWin(false), next->SetWin(true);
            }
            else {
                prev->setScore(prev->getScore() + m_curBet);
                next->setScore(next->getScore() - 2 * m_curBet);
                prev->SetWin(true), next->SetWin(false);
            }
        }
        emit playerStatusChanged(player, GameControl::Winning);
        return;
    }
    // 4. 牌没有出完,下一个玩家继续出牌
    m_currPlayer = player->getNextPlayer();
    m_currPlayer->preparePlayHand();
    emit playerStatusChanged(m_currPlayer, GameControl::ThinkingForPlayHand);
}


