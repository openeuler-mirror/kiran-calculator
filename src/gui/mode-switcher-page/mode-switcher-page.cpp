/**
* @Copyright (C) 2021 KylinSec Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; If not, see <http: //www.gnu.org/licenses/>.
*
* Author:     luoqing <luoqing@kylinos.com.cn>
*/
#include "mode-switcher-page.h"
#include "./ui_mode-switcher-page.h"
#include "general-enum.h"
#include "mode-list.h"

#include <QDebug>
#include <QListWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QAbstractItemView>

//bool m_flag=1;

ModeSwitcherPage::ModeSwitcherPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeSwitcherPage)
{
    ui->setupUi(this);
    ui->modeList->setCurrentRow(Mode_Label_Standard);
    ui->modeList->setMovement(QListView::Static);

    qDebug() << ui->modeList->currentItem();

    this->setVisible(false);

    connect(this,SIGNAL(clickList(int )),this,SLOT(activateAnimation()));
    connect(ui->modeList,&ModeList::itemSelectionChanged, this, &ModeSwitcherPage::modeListitemSelection);
    controlAnimation();
    //事件过滤器
//    ui->modeList->installEventFilter(this);
    qApp->installEventFilter(this);   //this 添加对象

    ui->modeList->setGridSize(QSize(108,40));
}


ModeSwitcherPage::~ModeSwitcherPage()
{
    delete ui;
}

//先获取全局坐标，然后将全局转化为局部坐标，判断点击位置是否包含在控件内
//事件过滤器是结束事件的意思,结束掉鼠标点击事件，不让它往下传
bool ModeSwitcherPage::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)                  //判断事件类型 鼠标点击事件
    {
        QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);    //强制转换事件类型
        QPoint globalPoint =  mouseEv->globalPos();
        QPoint point = ui->modeList->mapFromGlobal(globalPoint);
        QPoint point_2 = ui->modeListBlank->mapFromGlobal(globalPoint);
        if((!ui->modeList->rect().contains(point)) && (!ui->modeListBlank->rect().contains(point_2)))
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
    modeAnimation = new QPropertyAnimation(this, "geometry");
    modeAnimation->setDuration(300);
    // 起始位置
    modeAnimation->setStartValue(QRect(-108,0,108,550));
    // 终点位置
    modeAnimation->setEndValue(QRect(0,0,108,550));
    modeAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void ModeSwitcherPage::activateAnimation()
{
    this->setVisible(true);
    if(m_flag)
    {
        //设置动画方向
        modeAnimation->setDirection(QAbstractAnimation::Forward);
        qDebug() << modeAnimation->direction();
        modeAnimation->start();
        qDebug() << modeAnimation->state();
        m_flag = 0;

    }
    else if(!m_flag)
    {
        modeAnimation->setDirection(QAbstractAnimation::Backward);
        qDebug() << modeAnimation->direction();
        modeAnimation->start();
        qDebug() << modeAnimation->state();
        m_flag = 1;
    }
}

//当前选中模式,并发送信号
int ModeSwitcherPage::modeListitemSelection()
{

    int currentMode = ui->modeList->currentRow();

    //不能将非枚举量赋值给枚举变量，但能将枚举量赋值给非枚举变量
    //因为枚举量时符号常量，赋值编译器会自动把枚举量转为int类型
    if(Calculation_Mode_Standard == currentMode)
    {
        emit clickList(Calculation_Mode_Standard);
    }
    else if (Calculation_Mode_Science == currentMode)
    {
        emit clickList(Calculation_Mode_Science);
    }
    else if (Calculation_Mode_Programmer == currentMode)
    {
        emit clickList(Calculation_Mode_Programmer);
    }
    return currentMode;
}

