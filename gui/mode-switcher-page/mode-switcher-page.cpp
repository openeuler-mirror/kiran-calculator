#include "mode-switcher-page.h"
#include "./ui_mode-switcher-page.h"
#include "general-enum.h"
#include <QString>
#include <QDebug>
#include <QListWidget>

bool ModeSwitcherPage::m_flag=1;

ModeSwitcherPage::ModeSwitcherPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeSwitcherPage)
{
    //this指针指向CustomDrawer
    //利用ui指向内部组件
    ui->setupUi(this);

    ui->modeListWidget->setCurrentRow(Mode_Label_Standard);

    qDebug() << ui->modeListWidget->currentItem();

    this->setVisible(false);

    connect(this,SIGNAL(clickList(int )),this,SLOT(activateAnimation()));

    controlAnimation();
    //事件过滤器
    qApp->installEventFilter(this);   //this 添加对象
}


ModeSwitcherPage::~ModeSwitcherPage()
{
    delete ui;
}

#include <QEvent>
#include <QMouseEvent>

//先获取全局坐标，然后将全局转化为局部坐标，判断点击位置是否包含在控件内
//事件过滤器是结束事件的意思,结束掉鼠标点击事件，不让它往下传
bool ModeSwitcherPage::eventFilter(QObject *watched, QEvent *event)
{
//  QEvent::MouseButtonRelease
    if(event->type() == QEvent::MouseButtonPress)                  //判断事件类型 鼠标点击事件
    {
        QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);    //强制转换事件类型
        QPoint globalPoint =  mouseEv->globalPos();
        QPoint point = ui->modeListWidget->mapFromGlobal(globalPoint);
        if(!ui->modeListWidget->rect().contains(point))
        {
            if(!m_flag)
            {
                activateAnimation();
                return true;                                    //返回true,事件不会往下传递
            }
        }
    }
    return false;     //false-监听,true-公有                       //返回false,事件会往下传递
}

//控制动画
void ModeSwitcherPage::controlAnimation()
{
    modeListOpacity = new QGraphicsOpacityEffect(this);
    modeListOpacity->setOpacity(1);   //设置不透明度
    //ui->modeListWidget->setGraphicsEffect(modeListOpacity);
    this->setGraphicsEffect(modeListOpacity);

    //大小变化动画
    modeScaleAnimation = new QPropertyAnimation(this, "geometry");
    modeScaleAnimation->setDuration(1000);

//    pScaleAnimation1->setStartValue(QPoint(this->x(),100));
//    pScaleAnimation1->setEndValue(QPoint((this->x()),(this->y())));
//    pScaleAnimation1->setEndValue(QPoint(-width(), 0));

    // 起始位置
    modeScaleAnimation->setStartValue(QRect(0,0,0,416));
    // 终点位置
    modeScaleAnimation->setEndValue(QRect(0,0,400,416));

    modeScaleAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    //透明度变化动画
    modeOpacityAnimation = new QPropertyAnimation(modeListOpacity,"opacity");
    modeOpacityAnimation->setDuration(1000);
    modeOpacityAnimation->setStartValue(0);
    modeOpacityAnimation->setEndValue(1);
}

void ModeSwitcherPage::activateAnimation()
{
    //m_flag = 1;
    this->setVisible(true);
    if(m_flag)
    {
        //设置动画方向
        modeScaleAnimation->setDirection(QAbstractAnimation::Forward);
        modeOpacityAnimation->setDirection(QAbstractAnimation::Forward);

        //动画同时开始
        modeScaleAnimation->start();
        modeOpacityAnimation->start();
        m_flag = 0;
        qDebug() << "activateAnimation m_flag = false";

    }
    else if(!m_flag)
    {
        modeScaleAnimation->setDirection(QAbstractAnimation::Backward);
        modeOpacityAnimation->setDirection(QAbstractAnimation::Backward);
        modeScaleAnimation->start();
        modeOpacityAnimation->start();
        m_flag = 1;
        qDebug() << "activateAnimation m_flag = true";
    }
}

//当前选中模式,并发送信号
int ModeSwitcherPage::on_modeListWidget_itemSelectionChanged()
{
    //qDebug() << ui->modeListWidget->currentItem();
    int currentMode = ui->modeListWidget->currentRow();
    qDebug() << currentMode;

    //不能将非枚举量赋值给枚举变量，但能将枚举量赋值给非枚举变量
    //因为枚举量时符号常量，赋值编译器会自动把枚举量转为int类型
    if(Mode_Label_Standard == currentMode)
    {
        emit clickList(Mode_Label_Standard);
    }
    else if (Mode_Label_Science == currentMode)
    {
        emit clickList(Mode_Label_Science);
    }
    else if (Mode_Label_Programmer == currentMode)
    {
        emit clickList(Mode_Label_Programmer);
    }
    return currentMode;
}


//#include <QPainter>
//void CustomDrawer::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.fillRect(this->rect(),Qt::green);
//}
