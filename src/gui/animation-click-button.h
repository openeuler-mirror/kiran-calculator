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
#ifndef ANIMATIONCLICKBUTTON_H
#define ANIMATIONCLICKBUTTON_H
#include <QPushButton>
#include <QTimer>
class AnimationClickButton : public QPushButton
{
    Q_OBJECT
public:
    AnimationClickButton(const QString &text,QWidget *parent=nullptr );

    void animation();
private:
    QTimer m_timer;
};

#endif // ANIMATIONCLICKBUTTON_H
