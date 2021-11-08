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
#ifndef SCIENCEKEYSPAGE_H
#define SCIENCEKEYSPAGE_H
#include "keys-page.h"
#include <QWidget>
#include <QPushButton>
#include <QVariant>
#include <QObject>
#include <QGridLayout>

enum Button;
class QPushButton;
class AnimationClickButton;

class ScienceKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit ScienceKeysPage(QWidget *parent = nullptr);
    void initScienceButtons();

public slots:
    void handleScienceButtonClicked();
    void switchScienceAngleUnit();
    void switchScienceHYP();
    void shiftScienceFunction();
    void handleButtonAnimate(Button button);

signals:
    void scienceButtonClicked(Button button) const;
    void scienceSwitchAngleUnit(int angleUnit);
    void scienceIsShift();
    void scienceIsHYP();

private:
    QGridLayout* m_gridLayout = nullptr;
    QMap<Button, AnimationClickButton*> m_keyEnumMap;

    QPushButton* keySin;
    QPushButton* keyCos;
    QPushButton* keyTan;

    bool m_isShift = false;
    bool m_isHYP = false;
    int m_angleUnit = 0;
};

#endif // SCIENCEKEYSPAGE_H
