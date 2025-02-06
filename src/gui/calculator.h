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
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "standard-mode-page/standard-keys-page.h"
#include "keys-page.h"

#include <QWidget>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class StandardKeysPage;
class StandardExprCalculator;
class HistoryRecoder;
class Session;
class Evaluator;
class Settings;
enum Button;

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

    void initKeyButton();

public slots:
    void switchKeyborad(int mode);
    void loadStylesheet();

private:
    Ui::Calculator *ui;
    Evaluator* m_evaluator;
};
#endif // CALCULATOR_H
