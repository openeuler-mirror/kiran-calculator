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
#include "expr-calculator.h"
#include "core/evaluator.h"
#include "stage-page.h"
#include "utils.h"
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QDebug>
ExprCalculator::ExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    setAttribute(Qt::WA_InputMethodEnabled, false);  //禁止输入法
}

void ExprCalculator::setStagePage(StagePage *stage)
{
    m_stagePage = stage;
}

void ExprCalculator::setFuncList(QList<QString> funcList)
{
    m_funclist = funcList;
}

void ExprCalculator::clearStagePage()
{
    m_stagePage->clear();
}

bool ExprCalculator::expressionInFunc(QString &text)
{
    QString oldText = text;
    int curPos = cursorPosition();
    int epos = QLineEdit::text().indexOf("E");
    QString sRegNum = "[0-9,.E]";
    QRegExp rx;
    rx.setPattern(sRegNum);
    if (curPos == 0) {
        return false;
    }
    if (QLineEdit::text().length() > curPos && rx.exactMatch(QLineEdit::text().at(curPos))) {
        return false;
    }
    if (epos > -1 && epos == curPos - 1) {
        return false;
    }

    QString sRegNum1 = "[^0-9,.)πe]";
    QRegExp rx1;
    rx1.setPattern(sRegNum1);
    if (rx1.exactMatch(oldText.at(curPos - 1)))
        return false;
    else {
        QString newtext = QLineEdit::text();
        int percentpos = cursorPosition();
        int operatorpos =
            newtext.lastIndexOf(QRegularExpression(QStringLiteral("[^0-9,.eπE]")), percentpos - 1);    //"[^0-9,.epi]"

        bool nooperator = false;
        if (operatorpos > 0 && newtext.at(operatorpos - 1) == "E")
            operatorpos =
                newtext.mid(0, operatorpos - 1)
                .lastIndexOf(QRegularExpression(QStringLiteral("[^0-9,.eπE]")), percentpos - 1);
        /*出现以下情况：
         * 1. 负号在表达式的开头，如-1,-120等，视为一个整体的负数
         * 2. 负号在左括号的后一位，如(-1, (-121等，也视为一个整体的负数
         * 其中，当出现(-12)时，光标在右括号右侧时则会优先取到 ")",只有在右括号左侧才满足条件2*/
        if ((operatorpos == 0 && newtext.at(0) == "－")
                || (operatorpos > 0 && newtext.at(operatorpos) == "－" && newtext.at(operatorpos - 1) == "("))
            operatorpos--;

        if (operatorpos < 0) {
            operatorpos++;
            nooperator = true;
        }

        if (newtext.at(percentpos - 1) == ')') {
            if (operatorpos > 0 && newtext.at(operatorpos - 1) == '(')
                return false;
            do {
                operatorpos = newtext.lastIndexOf('(', operatorpos - 1);
                if (operatorpos <= 0) {
                    break;
                }
            } while (newtext.mid(operatorpos, newtext.size() - operatorpos).count('(') !=
                     newtext.mid(operatorpos, percentpos - operatorpos).count(')'));

            //匹配到的(不在开头且(左侧是字母
            QString sRegNum2 = "[A-Za-z]";
            QRegExp latterrx;
            latterrx.setPattern(sRegNum2);
            if (operatorpos > 0 && latterrx.exactMatch(QLineEdit::text().at(operatorpos - 1))) {
                int funpos = -1; //记录函数位
                int i;
                for (i = 0; i < m_funclist.size(); i++) {
                    //记录(左侧离(最近的函数位
                    funpos = QLineEdit::text().lastIndexOf(m_funclist[i], operatorpos - 1);
                    if (funpos != -1 && (funpos + m_funclist[i].length() == operatorpos))
                        break; //(在函数结尾
                    else
                        funpos = -1;
                }
                if (funpos != -1)
                    operatorpos = operatorpos - m_funclist[i].length(); //截取函数
            } else if (operatorpos > 1 && QLineEdit::text().mid(operatorpos - 2, 2) == "1/") {
                operatorpos = operatorpos - 2; //截取倒数
            }

            text = newtext.mid(operatorpos,
                               percentpos - operatorpos);  //截取表达式
        } else {
            text = newtext.mid(operatorpos + (nooperator == true ? 0 : 1),
                               percentpos - operatorpos + (nooperator == true ? 1 : 0) - 1);
            //截取表达式
        }
    }
    return true;
}

bool ExprCalculator::judgeInsertPos()
{
    QString sRegNum = "[A-Za-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);
    if (cursorPosition() > 0 && rx.exactMatch(text().at(cursorPosition() - 1))) {
        for (int i = 0; i < m_funclist.size(); i++) {
            //记录光标左侧离光标最近的函数位
            int funpos = text().lastIndexOf(m_funclist[i], cursorPosition() - 1);  //没找到函数名return -1
            if (funpos != -1 && (funpos < cursorPosition()) && (cursorPosition() < funpos + m_funclist[i].length()))
                return false; //光标在函数中
        }
    }
    return true;
}

void ExprCalculator::handleFunction_Backspace()
{
    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);
    if((!selectedText().isEmpty())&& !(selectedText() == QLineEdit::text()))
    {
        qDebug() << "选中时不可删除（暂定）";
        deselect(); //取消选中
        return;
    } else {
        QString text = QLineEdit::text();
        int cur = QLineEdit::cursorPosition();
        int funpos = -1;
        int i;
        if (text.size() > 0 && cur > 0 && text[cur - 1] == ",") {
            text.remove(cur - 2, 2);
            QLineEdit::setText(text);
            QLineEdit::setCursorPosition(cur - 2);
        } else {
            //退函数
            //光标不在开头且光标左侧是字母
            if (QLineEdit::cursorPosition() > 0 && rx.exactMatch(QLineEdit::text().at(QLineEdit::cursorPosition() - 1))) {
                for (i = 0; i < m_funclist.size(); i++) {
                    //记录光标左侧离光标最近的函数位
                    funpos = QLineEdit::text().lastIndexOf(m_funclist[i], QLineEdit::cursorPosition() - 1);
                    if (funpos != -1 && (funpos <= QLineEdit::cursorPosition())
                            && (QLineEdit::cursorPosition() <= funpos + m_funclist[i].length()))
                        break; //光标在函数开头和函数结尾之间
                    else
                        funpos = -1;
                }
                if (funpos != -1) {
                    QLineEdit::setText(QLineEdit::text().remove(funpos, m_funclist[i].length()));
                    QLineEdit::setCursorPosition(funpos);
                }
            } else {
                int proNumber = text.count(",");
                QLineEdit::backspace();
                int separator = proNumber - QLineEdit::text().count(",");
                int newPro = QLineEdit::text().count(",");
                if (cur > 0) {
                    QString sRegNum1 = "[0-9]+";
                    QRegExp rx1;
                    rx1.setPattern(sRegNum1);
                    //退数字
                    if (rx1.exactMatch(text.at(cur - 1)) && proNumber > newPro) {
                        if (text.mid(cur, text.length() - cur) == QLineEdit::text().mid(QLineEdit::text().length() - (text.length() - cur), text.length() - cur)) {
                            QLineEdit::setCursorPosition(cur - 2);
                        } else
                            QLineEdit::setCursorPosition(cur - 1);
                    } else {
                        if (separator < 0) {
                            QLineEdit::setCursorPosition(cur - 1 - separator);
                        } else {
                            QLineEdit::setCursorPosition(cur - 1);
                        }
                    }
                    //退小数点
                    if (text.at(cur - 1) == ".") {
                        if (text.mid(0, cur).count(",") != QLineEdit::text().mid(0, cur).count(","))
                            QLineEdit::setCursorPosition(cur);
                        else
                            QLineEdit::setCursorPosition(cur - 1);
                    }
                }
            }
        }
    }
}
//FIXEME:取反功能有问题，后续优化   例如：555 + sqrt(55)+44   5%
void ExprCalculator::handleFunction_Opposite()
{
    QString exptext = text();//表达式
    int curPos = cursorPosition();
    if (text() == QString()) {
        setText("(-");
        return;
    }
    if (!expressionInFunc(exptext)) {
        setText(exptext);
        setCursorPosition(curPos);
        return;
    } else {
        if (exptext.count("(") == exptext.count(")")) {
            setCursorPosition(curPos - exptext.length());
            insert("(-");
            int afterinsertpos = cursorPosition();
            setCursorPosition(afterinsertpos + exptext.length());
            insert(")");
        }
    }
}

void ExprCalculator::handleFunction_Reciprocal()
{
    if(text().isEmpty() ||text() == '0')
        return;
    else
    {
        QString  expr = "1/("+ text() + ")";
        clear();
        insert(expr);
    }
}

void ExprCalculator::insert(const QString &text)
{
    QLineEdit::insert(text);
}

void ExprCalculator::handleInsertText(const QString &text)
{
    if(!judgeInsertPos())
        return;
    if(text.isEmpty())
        return;
    if((!selectedText().isEmpty())&& !(selectedText() == QLineEdit::text()))      //全选时可以删除
    {
        deselect();
        return;
    }
    insert(text);
}

void ExprCalculator::setText(const QString &result)
{
    //显示结果中去除'='号和空格符号
    QString resultModify =  result;
    resultModify.remove("=").remove(" ").remove("\n");
    QLineEdit::setText(resultModify);
}

void ExprCalculator::reformatShowExpr(const QString &text)
{
    int oldPosition = this->cursorPosition();

    QString reformatExpr;
    reformatExpr = reformatExpr = Utils::reformatSeparators(QString(text).remove(" ").remove(","));
    setText(reformatExpr);
    autoZoomFontSize();  //缩放字体大小

    // reformat text.
    int oldLength = QString(text).length();
    int newLength = reformatExpr.length();

    int pos;
    if (newLength > oldLength)
        pos = oldPosition + (newLength - oldLength);
    else
        pos = oldPosition - (oldLength - newLength);
    if (pos > newLength)
        pos = newLength;

    this->setCursorPosition(pos);
}

void ExprCalculator::disableSelectText()
{
    qobject_cast<QLineEdit*>(sender())->deselect();
}


void ExprCalculator::autoZoomFontSize(int fontSize)
{
    QFont font("Noto Sans CJK SC Regular");
    for (int i = fontSize; i > 15 ; --i)
    {
        font.setPixelSize(i);
        QFontMetrics fm(font);
        qDebug() << "fm.lineSpacing()";
        qDebug() << fm.lineSpacing();

        int fontWidth = fm.width(text());
        int editWidth = width() - 45;

        if (fontWidth < editWidth)
            break;
    }

    setFont(font);
}
