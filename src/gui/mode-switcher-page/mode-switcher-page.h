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
#ifndef __MODE_SWITCHER_PAGE_H__
#define __MODE_SWITCHER_PAGE_H__

#include <QWidget>

class QPropertyAnimation;

namespace Ui {
class ModeSwitcherPage;
}

class ModeSwitcherPage : public QWidget
{
    Q_OBJECT

public:
    explicit ModeSwitcherPage(QWidget *parent = nullptr);
    ~ModeSwitcherPage();

    QPropertyAnimation *modeAnimation;

    // 事件过滤器
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void controlAnimation();

public slots:
    int  modeListitemSelection();
    void activateAnimation();

private:
    Ui::ModeSwitcherPage *ui;
    bool m_flag = true;
signals:
    void clickList(int);
    void clickList();

};

#endif // CUSTOMDRAWER_H
