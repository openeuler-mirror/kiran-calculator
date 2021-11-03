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
#include "standard-expr-calculator.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/sessionhistory.h"
#include "utils.h"
#include "general-enum.h"
#include "standard-keys-page.h"
#include <QDebug>
#include <qt5-log-i.h>

StandardExprCalculator::StandardExprCalculator(QWidget* parent) : ExprCalculator(parent)
{
    m_evaluator = Evaluator::instance();
    initMenuAndAction();
    //禁用输入栏菜单
//    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

    connect(this,SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this,SIGNAL(exprCalcMessageDec(const QString &)), this, SLOT(setText(const QString &)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));

    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));    //禁用选中
//    m_funclist = {"lg", "ln", "log","sqrt","%"};
    m_funclist = {"lg", "ln", "log","sqrt","+","-","e"};
    setFuncList(m_funclist);
}

void StandardExprCalculator::setSession(Session *session)
{
    m_standardSession = session;
    m_evaluator->setSession(m_standardSession);
}

void StandardExprCalculator::setStandardKeysPage(StandardKeysPage * keys)
{
    m_standardKeys = keys;
}

//传入QLineEdit::text()
//Evaluator类中包含运算规则和计算，在该类外部只做括号匹配检测
//内部没进行括号匹配，只进行将括号去除的操作
void StandardExprCalculator::exprCalc()
{
    QString expr = m_evaluator->autoFix(QLineEdit::text().remove(" ").remove(","));

    if (expr.isEmpty())
        return;
    // Same reason as above, do not update "ans".
    m_evaluator->setExpression(expr);

    //先字符合法检测，然后进入堆栈计算,并返回计算结果
    //包括：无效表达式、变量赋值、函数赋值、compile规则检查
    //auto quantity = m_evaluator->evalNoAssign();
    Quantity quantity = m_evaluator->evalUpdateAns();

    if (m_evaluator->error().isEmpty()) {
        if(!quantity.isNan())
        {
            //输出结果的进制转换
            //十进制
            auto formatDec = NumberFormatter::format(quantity);     //NumberFormatter和DMath结果的负号不一致

             // default:  Quantity::Format::Base::Decimal;  Mode::General; 另外可以通过设置Precison来精确位数
//            auto formatDec = DMath::format(quantity, Quantity::Format::Decimal() + Quantity::Format::General());
    //        auto messageDec = tr("%1").arg(formatDec);

            //结果超过16位自动转为科学计数法
            QString valueLenth = formatDec;
            if(valueLenth.remove(".").length() > 16)
            {
                formatDec = DMath::format(quantity,Quantity::Format::Scientific());
            }
            emit exprCalcMessageDec(formatDec);
            emit exprCalcQuantityDec(quantity);
            emit calculateMode(Calculation_Mode_Standard);
            qDebug() << formatDec;
            qDebug() << "formatDec:"+formatDec;
            //存入标准历史记录
            m_standardSession->addHistoryEntry(HistoryEntry(expr,quantity));
            emit historyChanged();
            //存入暂存记录
            emit stageExprFormat(expr);
            emit stageQuantity(quantity);
            emit stageChanged();
        }
        else
            emit exprCalcNan();
    } else
    {
//        emit exprCalcMessageDec(m_evaluator->error());
        emit exprCalcError();
    }

}

//按回车 和 (=号)  发射信号.进行计算
void StandardExprCalculator::triggerEnter()
{
    exprCalc();
    emit equalPressed();
    qDebug() << "returnPressed";
}

void StandardExprCalculator::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();
    qInfo() << "keyPressed:" << event;
    KLOG_INFO() << "keyPressed" <<event;

    switch (key) {
    case Qt::Key_0:
        handleInsertText("0");
        m_standardKeys->handleButtonAnimate(Button_Key_0);
        break;
    case Qt::Key_1:
        handleInsertText("1");
        m_standardKeys->handleButtonAnimate(Button_Key_1);
        break;
    case Qt::Key_2:
        handleInsertText("2");
        m_standardKeys->handleButtonAnimate(Button_Key_2);
        break;
    case Qt::Key_3:
        handleInsertText("3");
        m_standardKeys->handleButtonAnimate(Button_Key_3);
        break;
    case Qt::Key_4:
        handleInsertText("4");
        m_standardKeys->handleButtonAnimate(Button_Key_4);
        break;
    case Qt::Key_5:
        handleInsertText("5");
        m_standardKeys->handleButtonAnimate(Button_Key_5);
        break;
    case Qt::Key_6:
        handleInsertText("6");
        m_standardKeys->handleButtonAnimate(Button_Key_6);
        break;
    case Qt::Key_7:
        handleInsertText("7");
        m_standardKeys->handleButtonAnimate(Button_Key_7);
        break;
    case Qt::Key_8:
        handleInsertText("8");
        m_standardKeys->handleButtonAnimate(Button_Key_8);
        break;
    case Qt::Key_9:
        handleInsertText("9");
        m_standardKeys->handleButtonAnimate(Button_Key_9);
        break;

    case Qt::Key_Plus:
        handleInsertText("+");
        m_standardKeys->handleButtonAnimate(Button_Key_Add);
        break;
    case Qt::Key_Minus:
        handleInsertText("−");   //speedcrunch
//        insert(QString::fromUtf8("−"));
        m_standardKeys->handleButtonAnimate(Button_Key_Sub);
        break;

    case Qt::Key_Asterisk:
        handleInsertText("×");
        m_standardKeys->handleButtonAnimate(Button_Key_Mult);
        break;
    case Qt::Key_Slash:
        handleInsertText("÷");
        m_standardKeys->handleButtonAnimate(Button_Key_Divide);
        break;
    case Qt::Key_Period:
        handleInsertText(".");
        m_standardKeys->handleButtonAnimate(Button_Key_Point);
        break;
    case Qt::Key_Percent:
        handleInsertText("%");
        m_standardKeys->handleButtonAnimate(Button_Key_Percent);
        break;
    case Qt::Key_AsciiCircum:
        if(text().isEmpty())
            handleInsertText("0^");
        else
            handleInsertText("^");
        break;
    case Qt::Key_At:
        handleFunction_Sqrt();
        m_standardKeys->handleButtonAnimate(Button_Key_Sqrt);
        break;
    case Qt::Key_Q:
        handleFunction_Square();
        m_standardKeys->handleButtonAnimate(Button_Key_Square);
        break;
    case Qt::Key_R:
        handleFunction_Reciprocal();
        m_standardKeys->handleButtonAnimate(Button_Key_Reciprocal);
        break;
    case Qt::Key_Enter:
        exprCalc();
        m_standardKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Return:
        exprCalc();
        m_standardKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Equal:
        exprCalc();
        m_standardKeys->handleButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Backspace:
        handleFunction_Backspace();
        m_standardKeys->handleButtonAnimate(Button_Key_Backspace);
        break;
    case Qt::Key_Delete:
        clear();
        m_standardKeys->handleButtonAnimate(Button_Key_Clear);
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
        m_standardKeys->handleButtonAnimate(Button_Key_Clear);
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

void StandardExprCalculator::handleFunction_Sqrt()
{
    handleInsertText("sqrt()");
    cursorBackward(false);
}

void StandardExprCalculator::handleFunction_Square()
{
    if(!judgeInsertPos())
        return;
    if(text().isEmpty())
        handleInsertText("0^2");
    else
        handleInsertText("^2");
}

