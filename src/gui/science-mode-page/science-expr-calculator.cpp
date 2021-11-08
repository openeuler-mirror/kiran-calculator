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
#include "science-expr-calculator.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/sessionhistory.h"
#include "utils.h"
#include "general-enum.h"
#include "science-keys-page.h"
#include <QDebug>

ScienceExprCalculator::ScienceExprCalculator(QWidget* parent) : ExprCalculator(parent)
{
    Settings::instance()->angleUnit = 'd';
    Settings::instance()->complexNumbers = false;
    DMath::complexMode = false;                    //关闭复数模式
    m_evaluator = Evaluator::instance();

    connect(this,&ScienceExprCalculator::returnPressed,this,&ScienceExprCalculator::triggerEnter);
    connect(this, SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this, SIGNAL(exprCalcMessageDec(const QString&)), this, SLOT(setText(const QString&)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));

    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));

    m_funclist = {"arcsin", "arccos", "arctan","arsinh","arcosh","artanh",
                  "sin", "cos", "tan", "sinh","cosh","tanh",
                  "lg", "ln", "log", "mod", "sqrt", "yroot", "pi", "e",
                 };

    setFuncList(m_funclist);

}

void ScienceExprCalculator::setSession(Session *session)
{
    m_scienceSession = session;
    m_evaluator->setSession(m_scienceSession);
    m_evaluator->initializeBuiltInVariables();
}

void ScienceExprCalculator::setScienceKeysPage(ScienceKeysPage *keys)
{
    m_scienceKeys = keys;
}


void ScienceExprCalculator::scienceExprCalc()
{
    QString scienceExpr = m_evaluator->autoFix(QLineEdit::text().remove(" ").remove(","));

    if(scienceExpr.isEmpty())
        return;

    m_evaluator->setExpression(scienceExpr);
    Quantity quantity = m_evaluator->evalUpdateAns();
    QString  formatDec;
    if(m_evaluator->error().isEmpty())
    {
        if(!quantity.isNan())
        {
            if(m_FE)
                formatDec= DMath::format(quantity, Quantity::Format::Scientific());
            else
            {
                QString valueLenth = formatDec = NumberFormatter::format(quantity);
                if(valueLenth.remove(".").length() > 24)
                {
                    formatDec= DMath::format(quantity, Quantity::Format::Scientific());
                }
            }
            emit exprCalcMessageDec(formatDec);
            emit exprCalcQuantityDec(quantity);
            emit calculateMode(Calculation_Mode_Science);

            m_scienceSession->addHistoryEntry(HistoryEntry(scienceExpr,quantity));

            int indexHistory = m_scienceSession->historyToList().count() - 1;

            if((indexHistory != -1) && (m_FE))
            {
                m_FEIndexList << indexHistory;
            }
            emit scienceFEIndexList(m_FEIndexList);
            emit stageExprFormat(scienceExpr);
            emit stageQuantity(quantity);
            emit stageChanged();
            emit historyChanged();
        }
        else
            emit exprCalcNan();
    }
    else
    {
//        emit scienceExprCalcMessage(m_evaluator->error());
        emit exprCalcError();
    }
}

void ScienceExprCalculator::triggerEnter()
{
    scienceExprCalc();
    emit equalPressed();
    qDebug() << "scienceEqualPressed()";
}


void ScienceExprCalculator::handleFunction_Sqrt()
{
    if(m_isShift)
    {
        if(text().isEmpty() || text() == '0')
            return;
        else
        {
            QString  expr = "1/("+ text() + ")";
            clear();
            insert(expr);
        }
    }
    else
    {
        handleInsertText("sqrt()");
    }

}

void ScienceExprCalculator::handleFunction_Square()
{
    if(m_isShift)
    {
        if(text().isEmpty())
            handleInsertText("0^3");
        else
            handleInsertText("^3");
    }
    else
    {
        if(text().isEmpty())
            handleInsertText("0^2");
        else
            handleInsertText("^2");
    }

}

void ScienceExprCalculator::handleFunction_Xn()
{
    if(m_isShift)
    {
        if(text().isEmpty())
            insert("0yroot");
        else
            insert("yroot");
    }
    else
    {
        if(text().isEmpty())
            insert(QString::fromLatin1("0^"));
        else
            insert(QString::fromLatin1("^"));
    }
}

void ScienceExprCalculator::handleFunction_Sin()
{
    if(m_isShift && m_isHYP)
    {
        handleInsertText("arsinh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        handleInsertText("arcsin()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        handleInsertText("sinh()");
        cursorBackward(false);
    }
    else
    {
        handleInsertText("sin()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Cos()
{
    if(m_isShift && m_isHYP)
    {
        handleInsertText("arcosh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        handleInsertText("arccos()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        handleInsertText("cosh()");
        cursorBackward(false);
    }
    else
    {
        handleInsertText("cos()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Tan()
{
    if(m_isShift && m_isHYP)
    {
        handleInsertText("artanh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        handleInsertText("arctan()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        handleInsertText("tanh()");
        cursorBackward(false);
    }
    else
    {
        handleInsertText("tan()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Log()
{
    if(m_isShift)
    {
        handleInsertText("ln()");
         cursorBackward(false);
    }
    else
    {
        handleInsertText("lg()");
        cursorBackward(false);
    }
}


void ScienceExprCalculator::handleFunction_10n()
{
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);
    if(m_isShift)
    {
        if(tmp.right(1) >= '0' && tmp.right(1)<= '9')
            insert("×e^");
        else
            insert("e^");
    }
    else
    {
        if(tmp.right(1) >= '0' && tmp.right(1)<= '9')
            insert("×10^");
        else
            insert("10^");
    }

}

void ScienceExprCalculator::handleFunction_Pi()
{
    int pos = cursorPosition();
    qDebug() << "cursorPosition:";
    qDebug() << cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);

    if(tmp.right(1) >= '0' && tmp.right(1)<= '9')
        insert("×3.14159265358979323846");
    else
        insert("3.14159265358979323846");
}

void ScienceExprCalculator::handleFunction_Exp()
{
    //科学计数法，不是指数函数
    if(text().isEmpty())
        insert(QLatin1String("0e+"));
    else
        handleInsertText("e+");
}


void ScienceExprCalculator::handleFunction_Factorials()
{
    if(text().isEmpty())
        handleInsertText("0!");
    else
        handleInsertText("!");
}



void ScienceExprCalculator::handleFunction_SwitchAngleUnit(int angleUnit)
{
    // 'r': radian; 'd': degree; 'g': gradian
    switch (angleUnit) {
    case Angle_Unit_Degree:
        Settings::instance()->angleUnit = 'd';
        break;
    case Angle_Unit_Radian:
        Settings::instance()->angleUnit = 'r';
        break;
    case Angle_Unit_Gradian:
        Settings::instance()->angleUnit = 'g';
        break;
    }
}

void ScienceExprCalculator::handleFunction_FE()
{
    if(!m_FE)
        m_FE = true;
    else
        m_FE = false;
    emit scienceFEChanged();
}

void ScienceExprCalculator::handleFunction_Shift()
{
    if(!m_isShift)
        m_isShift = true;
    else
        m_isShift = false;
}

void ScienceExprCalculator::handleFunction_HYP()
{
    if(!m_isHYP)
        m_isHYP = true;
    else
        m_isHYP = false;
}


void ScienceExprCalculator::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();

    switch (key) {
    case Qt::Key_0:
        handleInsertText("0");
        m_scienceKeys->handleButtonAnimate(Button_Key_0);
        break;
    case Qt::Key_1:
        handleInsertText("1");
        m_scienceKeys->handleButtonAnimate(Button_Key_1);
        break;
    case Qt::Key_2:
        handleInsertText("2");
        m_scienceKeys->handleButtonAnimate(Button_Key_2);
        break;
    case Qt::Key_3:
        handleInsertText("3");
        m_scienceKeys->handleButtonAnimate(Button_Key_3);
        break;
    case Qt::Key_4:
        handleInsertText("4");
        m_scienceKeys->handleButtonAnimate(Button_Key_4);
        break;
    case Qt::Key_5:
        handleInsertText("5");
        m_scienceKeys->handleButtonAnimate(Button_Key_5);
        break;
    case Qt::Key_6:
        handleInsertText("6");
        m_scienceKeys->handleButtonAnimate(Button_Key_6);
        break;
    case Qt::Key_7:
        handleInsertText("7");
        m_scienceKeys->handleButtonAnimate(Button_Key_7);
        break;
    case Qt::Key_8:
        handleInsertText("8");
        m_scienceKeys->handleButtonAnimate(Button_Key_8);
        break;
    case Qt::Key_9:
        handleInsertText("9");
        m_scienceKeys->handleButtonAnimate(Button_Key_9);
        break;

    case Qt::Key_Plus:
        handleInsertText("+");
        m_scienceKeys->handleButtonAnimate(Button_Key_Add);
        break;
    case Qt::Key_Minus:
        handleInsertText("−");
        m_scienceKeys->handleButtonAnimate(Button_Key_Sub);
        break;
    case Qt::Key_Asterisk:
        handleInsertText("×");
        m_scienceKeys->handleButtonAnimate(Button_Key_Mult);
        break;
    case Qt::Key_Slash:
        handleInsertText("÷");
        m_scienceKeys->handleButtonAnimate(Button_Key_Divide);
        break;
    case Qt::Key_Period:
        handleInsertText(".");
        m_scienceKeys->handleButtonAnimate(Button_Key_Point);
        break;
    case Qt::Key_Percent:
        handleInsertText("%");
        break;
    case Qt::Key_ParenLeft:
        handleInsertText("(");
        m_scienceKeys->handleButtonAnimate(Button_Key_LeftBracket);
        break;
    case Qt::Key_ParenRight:
        handleInsertText(")");
        m_scienceKeys->handleButtonAnimate(Button_Key_RightBracket);
        break;
    case Qt::Key_E:
//        insert(QLatin1String("e"));
        handleFunction_Exp();
        m_scienceKeys->handleButtonAnimate(Button_Key_Exp);
        break;
    case Qt::Key_P:
        handleInsertText(QLatin1String("pi"));
        m_scienceKeys->handleButtonAnimate(Button_Key_Pi);
        break;
    case Qt::Key_Q:
        handleFunction_Square();
        m_scienceKeys->handleButtonAnimate(Button_Key_Square);
        break;
    case Qt::Key_R:
        handleFunction_Reciprocal();
        break;
    case Qt::Key_T:
        handleFunction_Tan();
        m_scienceKeys->handleButtonAnimate(Button_Key_Tan);
        break;
    case Qt::Key_S:
        handleFunction_Sin();
        m_scienceKeys->handleButtonAnimate(Button_Key_Sin);
        break;
    case Qt::Key_O:
        handleFunction_Cos();
        m_scienceKeys->handleButtonAnimate(Button_Key_Cos);
        break;
    case Qt::Key_L:
        handleFunction_Log();
        m_scienceKeys->handleButtonAnimate(Button_Key_Log);
        break;
    case Qt::Key_Y:
        if(text().isEmpty())
            handleInsertText("0^");
        else
            handleInsertText("^");
        m_scienceKeys->handleButtonAnimate(Button_Key_Xn);
        break;
    case Qt::Key_AsciiCircum:
        if(text().isEmpty())
            handleInsertText("0^");
        else
            handleInsertText("^");
        m_scienceKeys->handleButtonAnimate(Button_Key_Xn);
        break;
    case Qt::Key_At:
        handleInsertText("sqrt()");
        cursorBackward(false);
        m_scienceKeys->handleButtonAnimate(Button_Key_Sqrt);
        break;
    case Qt::Key_Exclam:
        handleFunction_Factorials();
        m_scienceKeys->handleButtonAnimate(Button_Key_Factorials);
        break;
    case Qt::Key_Enter:
        scienceExprCalc();
        m_scienceKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Return:
        scienceExprCalc();
        m_scienceKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Equal:
        scienceExprCalc();
        m_scienceKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Backspace:
        handleFunction_Backspace();
        m_scienceKeys->handleButtonAnimate(Button_Key_Backspace);
        break;
    case Qt::Key_Delete:
        clear();
        m_scienceKeys->handleButtonAnimate(Button_Key_Clear);
        break;
    case Qt::Key_Left:
        setCursorPosition(cursorPosition() - 1);
        break;
    case Qt::Key_Right:
        setCursorPosition(cursorPosition() + 1);
        break;
    case Qt::Key_Escape:
        clear();
        clearStagePage();
        m_scienceKeys->handleButtonAnimate(Button_Key_Clear);
        break;
    }
    //暂时禁用复制、粘贴、全选
//    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C))
//    {
//        copyResultToClipboard();
//    }
//    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_V))
//    {
//        paste();
//    }
//    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_A))
//    {
//        exprSelectAll();
//    }
}
