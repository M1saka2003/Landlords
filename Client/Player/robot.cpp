#include "robot.h"
#include "strategy.h"
#include "robotgraplord.h"
#include "robotplayhand.h"
#include <iostream>

Robot::Robot(QObject *parent)
    : Player{parent} {
    m_type = Player::Robot;
}

void Robot::prepareCallLord() {
    RobotGrapLord *subThread = new RobotGrapLord(this);
    connect(subThread, &RobotGrapLord::finished, this, [=]() {
        subThread->deleteLater();
    });
    subThread->start();
}

void Robot::preparePlayHand() {
    RobotPlayHand *subThread = new RobotPlayHand(this);
    connect(subThread, &RobotGrapLord::finished, this, [=]() {
        subThread->deleteLater();
    });
    subThread->start();
}

void Robot::thinkCallLord() {
    Strategy st(this, m_cards);
    int weight = 0;
    weight += st.getRangeCards(Card::Card_SJ, Card::Card_BJ).cardCount() * 6;

    QVector <Cards> optSeq = st.pickOptimalSeqSingles();
    weight += optSeq.size() * 5;

    QVector <Cards> bombs = st.findCardsByCount(4);
    weight += bombs.size() * 5;

    Cards tmp = m_cards;
    tmp.remove(optSeq);
    tmp.remove(bombs);
    Cards card2 = st.getRangeCards(Card::Card_2, Card::Card_2);
    tmp.remove(card2);

    QVector <Cards> Triple = Strategy(this, tmp).findCardsByCount(3);
    weight += Triple.size() * 4;

    tmp.remove(Triple);

    QVector <Cards> pairs = Strategy(this, tmp).findCardsByCount(2);
    weight += pairs.size() * 1;

    if (weight >= 22) {
        grabLordBet(3);
    }
    else if (weight < 22 && weight >= 18) {
        grabLordBet(2);
    }
    else if (weight < 18 && weight >= 10) {
        grabLordBet(1);
    }
    else {
        grabLordBet(0);
    }
}

void Robot::thinkPlayHand() {
    Strategy st(this, m_cards);
    Cards cs = st.makeStrategy();
    playHand(cs);
}

