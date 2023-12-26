#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QMainWindow>
#include <qlabel.h>
#include <QTimer>
#include "gamecontrol.h"
#include "cardpanel.h"
#include "animationwindow.h"
#include "countdown.h"
#include "bgmcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GamePanel; }
QT_END_NAMESPACE

class GamePanel : public QMainWindow {
    Q_OBJECT

public:
    GamePanel(QWidget *parent = nullptr);

    ~GamePanel();

    enum AnimationType {
        ShunZi,
        LianDui,
        Plane,
        JokerBomb,
        Bomb,
        Bet
    };

    // 初始化游戏控制类信息
    void GameControlInit();

    // 更新分数面板的分数
    void updatePlayerScore();

    // 切割并存储图片
    void initCardMap();

    // 裁剪图片
    void cropImage(QPixmap &pix, int x, int y, Card &c);

    // 初始化游戏按钮组
    void initButtonsGroup();

    // 初始化玩家在窗口中的上下文环境
    void initPlayerContext();

    // 初始化游戏场景
    void initGameScene();

    // 处理游戏的状态
    void gameStatusPress(GameControl::GameStatus status);

    // 发牌
    void starDispatchCard();

    // 移动扑克牌
    void cardMoveStep(Player *player, int curPos);

    // 处理分发得到的扑克牌
    void disposCard(Player *player, Cards &cards);

    // 处理玩家选牌
    void onCardSelected(Qt::MouseButton button);

    // 处理用户出牌
    void onUserPlayHand();

    // 放弃出牌
    void onUserPass();

    // 更新扑克牌在窗口中的显示
    void updatePlayerCards(Player *player);

    // 加载玩家头像
    QPixmap loadRoleImage(Player::Sex sex, Player::Direction dirct, Player::Role role);

    // 定时器的处理动作
    void onDispatchCard();

    // 处理玩家状态的变化
    void onPlayerStatusChanged(Player *player, GameControl::PlayerStatus status);

    // 处理玩家抢地主
    void onGrabLordBet(Player *player, int bet, bool falg);

    // 处理玩家出的牌
    void onDisposePlayHand(Player *player, Cards &cards);

    // 显示特效动画
    void showAnimation(AnimationType type, int bet = 0);

    // 隐藏打出的牌
    void hidePlayerDropCards(Player *player);

    // 显示玩家的最终得分
    void showEndingScroePanel();

    // 初始化闹钟倒计时
    void initCountDown();

protected:
    void paintEvent(QPaintEvent *ev) override;

    void mouseMoveEvent(QMouseEvent *ev) override;

private:
    enum CardAlign {
        Horizontal, Vertical
    };
    struct PlayerContext {
        // 1. 玩家扑克牌显示的区域
        QRect cardRect;
        // 2. 出牌的区域
        QRect playHandRect;
        // 3. 扑克牌的对齐方式(水平 or 垂直)
        CardAlign align;
        // 4. 扑克牌显示正面还是背面
        bool isFrontSide;
        // 5. 游戏过程中的提示信息, 比如: 不出
        QLabel *info;
        // 6. 玩家的头像
        QLabel *roleImg;
        // 7. 玩家刚打出的牌
        Cards lastCards;
    };

    Ui::GamePanel *ui{};
    QPixmap m_bkImage{};
    GameControl *m_gameCtl{};
    QVector<Player *> m_playerList{};
    QMap<Card, CardPanel *> m_cardMap{}; // 储存了所有玩家的扑克牌对象
    QSize m_cardSize{};
    QPixmap m_cardBackImg{};
    QMap<Player *, PlayerContext> m_contextMap{}; // 保存了各个玩家的窗口信息
    CardPanel *m_baseCard{};
    CardPanel *m_moveCard{};
    QVector<CardPanel *> m_last3Card{};
    QPoint m_baseCardPos{}; // 开始扑克牌的位置坐标
    GameControl::GameStatus m_gameStatus{};// 当前的游戏状态
    QTimer *m_timer{};
    AnimationWindow *m_animation{};
    CardPanel *m_curSelCard{};
    QSet<CardPanel *> m_selectCards{};
    QRect m_cardsRect{};
    QHash<CardPanel *, QRect> m_userCards{};
    CountDown *m_countDown{};
    BGMControl *m_bgm{};
};

#endif // GAMEPANEL_H
