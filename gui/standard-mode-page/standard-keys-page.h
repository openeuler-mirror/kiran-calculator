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
#ifndef STANDARDKEYSPAGE_H
#define STANDARDKEYSPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QObject>
#include "keys-page.h"

enum Button;
class QPushButton;
class QGridLayout;

class StandardKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit StandardKeysPage(QWidget *parent = nullptr);

    void initButtons();

public slots:
    void handleButtonClicked();

protected slots:
    void emitButtonPressed(int button) const;
//    void paintEvent(QPaintEvent *event) override;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
        QString objectName;
    }keyDescriptions[];

    QGridLayout* m_gridLayout = nullptr;

signals:
    void buttonPressed(Button) const;

};

#endif // STANDARDKEYSPAGE_H
