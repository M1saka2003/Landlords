#ifndef ROBOTGRAPLORD_H
#define ROBOTGRAPLORD_H

#include "player.h"
#include <QObject>
#include <QThread>

class RobotGrapLord : public QThread {
    Q_OBJECT

public:
    explicit RobotGrapLord(Player *plyaer, QObject *parent = nullptr);

protected:
    void run() override;

signals:

private:
    Player *m_player;
};

#endif // ROBOTGRAPLORD_H
