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
#ifndef ExprCalculator_H
#define ExprCalculator_H

#include <QWidget>
#include "core/session.h"
#include "expr-calculator.h"
#include <QLineEdit>
#include <QKeyEvent>

class Evaluator;
class Session;
class CNumber;
class QEvent;
class QKeyEvent;
class QWidget;
class HistoryEntry;
class Settings;
class Session;
class StandardKeysPage;

class StandardExprCalculator : public ExprCalculator
{
    Q_OBJECT
public:
    StandardExprCalculator(QWidget *parent = nullptr);

    void setStandardKeysPage(StandardKeysPage*);
    void setSession(Session*);

    void handleFunction_Sqrt();
    void handleFunction_Square();

protected:
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void exprCalc();

protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;
    Session* m_standardSession;
    QList<QString> m_funclist;
    StandardKeysPage* m_standardKeys;

};

#endif // ExprCalculator_H
