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

class ScienceKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit ScienceKeysPage(QWidget *parent = nullptr);
    void initScienceButtons();

    static int m_angleUnit;
    static bool m_isShift;
    static bool m_isHYP;

public slots:
    void handleScienceButtonClicked();
    void switchScienceAngleUnit();
    void switchScienceHYP();
    void shiftScienceFunction();

protected slots:
    void emitScienceButtonPressed(int button) const;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
        QString objectName;
        int rowcount;  //所占行数
        int columncount;  //所占列数
    }keyDescriptions[];

    QGridLayout* m_gridLayout = nullptr;
    QMap<Button, QPushButton*> m_keyEnumMap;

    QPushButton* keySin;
    QPushButton* keyCos;
    QPushButton* keyTan;

signals:
    void scienceButtonPressed(Button) const;
    void scienceSwitchAngleUnit(int );
    void scienceIsShift();
    void scienceIsHYP();
};

#endif // SCIENCEKEYSPAGE_H
