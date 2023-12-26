#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QEvent>
#include <QPushButton>

class MyButton : public QPushButton {
    Q_OBJECT

public:

    explicit MyButton(QWidget *parent = nullptr);

    void setImage(QString normal, QString hover, QString pressed);


protected:
    // 鼠标按下
    void mousePressEvent(QMouseEvent *ev) override;

    // 鼠标释放
    void mouseReleaseEvent(QMouseEvent *ev) override;

    // 鼠标进入
    void enterEvent(QEvent *ev) override;

    // 鼠标离开
    void leaveEvent(QEvent *ev) override;

    // 绘图
    void paintEvent(QPaintEvent *ev) override;

signals:

private:
    // QString 存储的是图片的路径
    QString m_normal;
    QString m_hover;
    QString m_pressed;

    QPixmap m_pixmap;
};

#endif // MYBUTTON_H
