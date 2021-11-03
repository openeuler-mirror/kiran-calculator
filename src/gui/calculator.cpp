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

#include "calculator.h"
#include "./ui_calculator.h"

#include "history-recoder.h"
#include "mode-switcher-page/mode-switcher-page.h"
#include "standard-mode-page/standard-keys-page.h"
#include "standard-mode-page/standard-expr-calculator.h"
#include "programmer-mode-page/num-conversion.h"

#include "core/session.h"
#include "core/evaluator.h"
#include "core/settings.h"
#include "general-enum.h"

#include <QMessageBox>
#include <QDebug>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), ui(new Ui::Calculator)
{
    ui->setupUi(this);
    //并不一定后初始化，在上层
    //m_drawer->raise(); 通过raise将图层放在上面
    //通过new得到的控件，默认显示在比它new的早的控件上面

    //    m_settings = Settings::instance();

    m_evaluator = Evaluator::instance();
    m_evaluator->initializeBuiltInVariables();

    m_drawer = new ModeSwitcherPage(this);

    //侧边栏按钮布局，待确定
    //侧边栏按钮在最上层
    QPushButton *m_drawerButton = new QPushButton(this);
    m_drawerButton->setObjectName("drawerButton");
    m_drawerButton->show();
    m_drawerButton->move(7, 7);
    m_drawerButton->resize(16, 16);
    m_drawerButton->setIcon(QIcon(":/kiran-calculator-images/kc-mode.svg"));
    m_drawerButton->setIconSize(QSize(16, 16));
    m_drawerButton->setFocusPolicy(Qt::NoFocus);

    connect(m_drawer, SIGNAL(clickList(int)), this, SLOT(modeLabelChanged(int)));
    connect(m_drawer, SIGNAL(clickList(int)), this, SLOT(switchKeyborad(int)));
    connect(m_drawerButton, SIGNAL(clicked()), this, SLOT(drawerButton()));

    ui->stackedWidget->setCurrentIndex(0);
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::modeLabelChanged(int mode)
{
    if (mode == Calculation_Mode_Standard)
    {
        ui->modeLabel->setText(tr("Standard"));
    }

    if (mode == Calculation_Mode_Science)
    {
        ui->modeLabel->setText(tr("Science"));
    }

    if (mode == Calculation_Mode_Programmer)
    {
        ui->modeLabel->setText(tr("Programmer"));
    }

    //    if(mode == Mode_Label_About) {ui->modeLabel->setText("关于");}
}

//不同模式下的键盘切换
void Calculator::switchKeyborad(int mode)
{
    ui->stackedWidget->setCurrentIndex(mode);
}

//缩小时，隐藏历史记录栏
//void Window::resizeEvent(QResizeEvent *event)
//{
//    qDebug() << this->frameGeometry().width();
//    int width = this->frameGeometry().width();
//}

//点击切换按钮
void Calculator::drawerButton()
{
    m_drawer->activateAnimation();
}
