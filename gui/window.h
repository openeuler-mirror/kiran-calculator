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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPushButton>

#include "standard-mode-page/standard-keys-page.h"
#include "keys-page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class ModeSwitcherPage;
class StandardKeysPage;
class ExprCalculator;
class HistoryRecoder;
class Session;
class Evaluator;
class Settings;
enum Button;


class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

//    void resizeEvent(QResizeEvent *event) override;
    void initKeyButton();

private:
    Ui::Window *ui;
    ModeSwitcherPage* m_drawer = nullptr;

//    StandardKeysPage* m_standardKeys = nullptr;
//    ExprCalculator* ui->standardExprEdit = nullptr;
//    HistoryRecoder * m_historyRecoder = nullptr;
//    Session *m_standardSession = nullptr;
//    Session *m_scienceSession = nullptr;
    Evaluator* m_evaluator;
//    Settings* m_settings;


public slots:
    void on_modeLabelChanged(int );
    void on_switchKeyborad(int );
    void on_drawerButton();
    //void handleStandardKeysButtonPress(Button);
    //void handleScienceKeysButtonPress(Button);

private slots:

signals:

};
#endif // WIDGET_H
