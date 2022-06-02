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
#ifndef SCIENCEEXPRCALCULATOR_H
#define SCIENCEEXPRCALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include "core/sessionhistory.h"
#include "core/session.h"
#include <QKeyEvent>
#include "expr-calculator.h"

class Evaluator;
class Session;
class CNumber;
class QEvent;
class QKeyEvent;
class QWidget;
class HistoryEntry;
class Settings;
class ScienceKeysPage;
class ScienceExprCalculator : public ExprCalculator
{
    Q_OBJECT
public:
    ScienceExprCalculator(QWidget* parent = nullptr);
    void setSession(Session* session);
    void setScienceKeysPage(ScienceKeysPage* keys);

    void handleFunction_10n();
    void handleFunction_Sqrt();
    void handleFunction_Square();
    void handleFunction_Xn();
    void handleFunction_Sin();
    void handleFunction_Cos();
    void handleFunction_Tan();
    void handleFunction_Log();
    void handleFunction_Pi();

    //XXX:后续优化科学模式下输入科学计数法以及输入指数函数,自然常数
    void handleFunction_Exp();         //科学计数法，不是指数函数
    void handleFunction_Factorials();

public slots:
    void scienceExprCalc();
    void handleFunction_SwitchAngleUnit(int angleUnit);
    void handleFunction_FE();
    void handleFunction_Shift();
    void handleFunction_HYP();

protected slots:
    void triggerEnter();

signals:
    void scienceFEChanged();
    void scienceFEIndexList(QList<int> indexList);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    
private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_scienceHistory;
    int m_currentScienceHistoryIndex;
    Session* m_scienceSession;
    QList<QString> m_funclist;
    QList<int> m_FEIndexList;
    ScienceKeysPage* m_scienceKeys;

    bool m_FE = false;
    bool m_isShift = false;
    bool m_isHYP = false;


};

#endif // SCIENCEEXPRCALCULATOR_H
