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
#ifndef PROGRAMMERKEYSPAGE_H
#define PROGRAMMERKEYSPAGE_H
#include "keys-page.h"
#include <QWidget>
#include <QPushButton>
#include <QVariant>
#include <QGridLayout>
#include <QMap>
enum Button;

class ProgrammerKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProgrammerKeysPage(QWidget *parent = nullptr);
    void initProgrammerButtons();
    void switchProgrammerLogicalAndShift();

    static bool m_isShifted;

public slots:
    void handleProgrammerButtonClicked();
    void switchProgrammerFormatKeys(int );

protected slots:
    void emitProgrammerButtonPressed(int button) const;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
        QString objectName;
    }keyDescriptions[];

    QGridLayout* m_gridLayout;
    QMap<Button, QPushButton*> m_keyEnumMap;
signals:
    void programmerButtonPressed(Button) const;
    void programmerIsShift();

};
#endif // PROGRAMMERKEYSPAGE_H
