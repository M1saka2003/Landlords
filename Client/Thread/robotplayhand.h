#ifndef ROBOTPLAYHAND_H
#define ROBOTPLAYHAND_H

#include "player.h"

#include <QObject>
#include <QThread>

class RobotPlayHand : public QThread {
    Q_OBJECT

public:
    explicit RobotPlayHand(Player *player, QObject *parent = nullptr);


signals:
protected:
    void run() override;

    Player *m_player;
};

#endif // ROBOTPLAYHAND_H
