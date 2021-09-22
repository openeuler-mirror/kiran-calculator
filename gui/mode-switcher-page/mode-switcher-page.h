#ifndef __MODE_SWITCHER_PAGE_H__
#define __MODE_SWITCHER_PAGE_H__

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QAbstractItemView>

namespace Ui {
class ModeSwitcherPage;
}

class ModeSwitcherPage : public QWidget
{
    Q_OBJECT

public:
    explicit ModeSwitcherPage(QWidget *parent = nullptr);
    ~ModeSwitcherPage();

    QGraphicsOpacityEffect *modeListOpacity;
    QPropertyAnimation *modeScaleAnimation;
    QPropertyAnimation *modeOpacityAnimation;

    // 事件过滤器
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void controlAnimation();

    static bool m_flag;
protected:
//    virtual void moveEvent(QMoveEvent *event) override;

public slots:
    int  modeListitemSelection();
    void activateAnimation();

private:
    Ui::ModeSwitcherPage *ui;

signals:
    void clickList(int);
    void clickList();

};

#endif // CUSTOMDRAWER_H
