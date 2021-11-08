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
#ifndef PROGRAMMEREXPRCALCULATOR_H
#define PROGRAMMEREXPRCALCULATOR_H
#include <QLineEdit>
#include <QWidget>
#include "core/session.h"
#include "core/sessionhistory.h"
#include "expr-calculator.h"
#include <QKeyEvent>

class Evaluator;
class Session;
class CNumber;
class HistoryEntry;
class Settings;
class ProgrammerKeysPage;
class StagePage;
class ProgrammerExprCalculator : public ExprCalculator
{
    Q_OBJECT
public:
    ProgrammerExprCalculator(QWidget* parent = nullptr);

    void setSession(Session* session);
    void setProgrammerKeysPage(ProgrammerKeysPage* keys);

    QString AddPrefixForExpr(int format, const QString& text);
    bool curposInNumber(int curpos);
    void handleProgrammerFunction_Opposite();
    void handleProgrammerFunction_Backspace();

    bool isNumber(QChar a);  //判断是否为数字(分隔符)

public slots:
    void handleProgrammerFunction_Shift();
    void handleProgrammerFunction_Lsh();
    void handleProgrammerFunction_Rsh();

    bool isNumberOutOfRange(const QString& text);
    void setText(const QString& result);
    void programmerExprCalc();
    void autoProgrammerExprCalc();
    void exprFormatChanged(int format);
    void radixChanged(int basedest);
    void reformatShowExpr(const QString& text);
    bool judgeInsertPos();  //判断光标位置，禁止在函数名中间插入
    QString scanAndExec(int previousFormat, int cureentFormat, const QString& text);

protected slots:
    void triggerEnter();

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;

signals:
    void programmerToNumConversionFormatDec(const QString& formatDec);
    void programmerToNumConversionQuantityDec(const Quantity& quantity);
    void programmerToNumConversionMessageError();
    void programmerExprIsEmpty();

private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_programmerHistory;
    int m_currentProgrammerHistoryIndex;
    Session* m_programmerSession;
    ProgrammerKeysPage* m_programmerKeysPage;
    QVector<QString> m_numvec;
    QVector<QString> m_opvec;
    QString m_textorder;
    QString m_tmpPreviousExpr;
    QList<QString> m_funclist;

    int m_currentFormat;
    int m_previousFormat;
    bool m_numOutRange;
    bool m_isShift;
};

#endif  // PROGRAMMEREXPRCALCULATOR_H
