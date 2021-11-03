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
\
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
//        QString exptext;  //表达式
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

void ExprCalculator::initMenuAndAction()
{
    m_menu = new QMenu(this);
//    m_copy = new QAction(tr("copy"),this);
    m_copy = new QAction(this);
    m_paste = new QAction(this);
    m_selectAll = new QAction(this);

    m_copy->setText("复制");
    m_paste->setText("粘贴");
    m_selectAll->setText("全选");

    connect(m_copy,SIGNAL(triggered()), this, SLOT(copyResultToClipboard()));
    connect(m_paste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(m_selectAll, SIGNAL(triggered()), this, SLOT(exprSelectAll()));
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
//取反功能有问题，考虑暂时不修改了，或者延后再做   例如：555 + sqrt(55)+44   5%
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
        qDebug() << "exptext";
        qDebug() << exptext;
        if (exptext.count("(") == exptext.count(")")) {
            qDebug() << "exptext____:";
            qDebug() << exptext;

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
//        pos = oldPosition;
        pos = oldPosition - (oldLength - newLength);
    if (pos > newLength)
        pos = newLength;

    this->setCursorPosition(pos);
}

void ExprCalculator::disableSelectText()
{
    qobject_cast<QLineEdit*>(sender())->deselect();
}

void ExprCalculator::showMenu(const QPoint &point)
{
    m_menu->addAction(m_copy);
    m_menu->addAction(m_paste);
    m_menu->addSeparator();
    m_menu->addAction(m_selectAll);

    if(QApplication::clipboard()->text().isEmpty())
        m_paste->setEnabled(false);
    else
        m_paste->setEnabled(true);

    if(selectedText().isEmpty())
        m_copy->setEnabled(false);
    else
        m_copy->setEnabled(true);

    if(text().isEmpty())
        m_selectAll->setEnabled(false);
    else
        m_selectAll->setEnabled(true);

    m_menu->exec(mapToGlobal(point));
}

void ExprCalculator::copyResultToClipboard()
{
    if(text().isEmpty())
        return;
    QApplication::clipboard()->setText(selectedText());
    qDebug() << QApplication::clipboard()->text();
}

void ExprCalculator::paste()
{
    if(!judgeInsertPos())
        return;
    if((!selectedText().isEmpty())&& !(selectedText() == QLineEdit::text()))
    {
        deselect();     //选中时不可以粘贴,全选时可以
        return;
    }

    QString oldText = text(); //未粘贴操作的text
    int curpos = cursorPosition(); //未粘贴操作的光标位

    QString exp = text();
    QString text = Utils::toHalfWidth(QApplication::clipboard()->text());
    text = text.left(text.indexOf("="));
    text = text.replace(QString::fromUtf8("＋"),"+")
               .replace(QString::fromUtf8("－"),QString::fromUtf8("−"))
               .replace("_", QString::fromUtf8("−"))
               .replace("-",QString::fromUtf8("−"))
               .replace('*', "×")
               .replace('X', "×")
               .replace(QString::fromUtf8("＊"), "×")
               .replace(QString::fromUtf8("（"), "(")
               .replace(QString::fromUtf8("）"), ")")
               .replace(QString::fromUtf8("——"), QString::fromUtf8("−"))
               .replace(QString::fromUtf8("％"), "%")
               .replace('/', "÷")
               .replace(QString::fromUtf8("。"), ".")
               .replace(QString::fromUtf8("π"), "pi")
               .replace("E", "e");

    //匹配函数方法
    QStringList list = text.split(QRegExp("[0-9+−×÷/()%^.,。]"));   //standard
//    QStringList list = text.split(QRegExp("[0-9+−×÷/()%^!e.,。]")); // science

    //根据不同进制，对应不同的筛选规则
    // \\s ->通过 \和s 意义不明
    /*
    switch (m_currentFormat) {
    case Num_Format_Hex:
        text.remove(QRegExp("[G-Z]"));
        list = text.split(QRegExp("[A-F0-9+−×÷()%\\s]"));
        break;
    case Num_Format_Oct:
        text.remove(QRegExp("[8-9]"));
        list = text.split(QRegExp("[0-7+−×÷()%\\s]"));
        break;
    case Num_Format_Bin:
        text.remove(QRegExp("[2-9]"));
        list = text.split(QRegExp("[0-1+−×÷()%\\s]"));
        break;
    default:
        list = text.split(QRegExp("[0-9+−×÷(),%\\s]"));
        break;
    }
    */
    for (int i = 0; i < list.size(); i++) {
        QString item = list[i];
        for (int j = 0; j < m_funclist.size(); j++) {
            if (item.toLower().contains(m_funclist[j])) {
                item.replace(item, m_funclist[j]); //包含函数名的取出;item中若存在两个函数名，只可以替代最前面的函数名
                break;
            }
            if (j == m_funclist.size() - 1)
                item.replace(item, QString());
        }
        text.replace(list[i], item);
    }

    while (exp.count("(") + text.count("(") > 100) {
        text.remove(text.lastIndexOf("("), 1);
    }
    while (exp.count(")") + text.count(")") > 100) {
        text.remove(text.lastIndexOf(")"), 1);
    }

    handleInsertText(text);
    if (QLineEdit::text() == exp) {
        setText(oldText);
        setCursorPosition(curpos);
        qDebug() << "Invalid content"; //提示是否复制了无效内容,复制的内容全是字母等
    }
}

void ExprCalculator::exprSelectAll()
{
    QLineEdit::selectAll();
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
