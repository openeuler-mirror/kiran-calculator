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
#include "programmer-expr-calculator.h"
#include "programmer-keys-page.h"
#include "stage-page.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/sessionhistory.h"
#include "utils.h"
#include "general-enum.h"
#include <QDebug>
#include <QRegularExpression>
#include <QDateTime>
#include <QRegExp>

#define NUM_FORMAT_DEC 1
#define BASE_TAG(basetag) \
    if (m_currentFormat == 0) \
        basetag = "0x"; \
    else if (m_currentFormat == 2) \
        basetag = "0o"; \
    else if (m_currentFormat == 3) \
        basetag = "0b"; \

const QString AtoF = "ABCDEF";
int m_currentFormat = 1;
int m_previousFormat = 1;
bool m_numOutRange = false;
bool m_isShift = false;

ProgrammerExprCalculator::ProgrammerExprCalculator(QWidget *parent) : ExprCalculator(parent)
{
    connect(this, SIGNAL(returnPressed()), this, SLOT(triggerEnter()));

    //手动触发计算
    connect(this, SIGNAL(exprCalcMessageHex(const QString& )), this, SLOT(setText(const QString& )));
    connect(this, SIGNAL(exprCalcMessageDec(const QString& )), this, SLOT(setText(const QString& )));
    connect(this, SIGNAL(exprCalcMessageOct(const QString& )), this, SLOT(setText(const QString& )));
    connect(this, SIGNAL(exprCalcMessageBin(const QString& )), this, SLOT(setText(const QString& )));

    //textChanged和textEdited信号不同在于，当以编程方式更改文本时，例如，通过调用setText()会发出textChanged信号，不会发出textEdited信号。

//    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(reformatShowExpr(const QString&)));
//    connect(this,SIGNAL(textEdited(const QString&)),this,SLOT(autoProgrammerExprCalc()));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(autoProgrammerExprCalc()));
    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));

    m_funclist = { "not", "xor", "and", "nor", "ror", "rol",
                      "shl", "shr", "or", "mod",
                 };
    setFuncList(m_funclist);
    initMenuAndAction();
//    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showMenu(const QPoint&)));
}

void ProgrammerExprCalculator::setSession(Session *session)
{
    m_programmerSession = session;
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_programmerSession);
}

void ProgrammerExprCalculator::setProgrammerKeysPage(ProgrammerKeysPage *keys)
{
    m_programmerKeysPage = keys;
}

void ProgrammerExprCalculator::exprFormatChanged(int format)
{
    m_previousFormat = m_currentFormat;      //记录转变之前的进制
    m_currentFormat = format;
}

//当进制表切换时，输入栏中对应的值需要同步切换
void ProgrammerExprCalculator::radixChanged(int cureentFormat)
{
    this->setText(scanAndExec(m_previousFormat,cureentFormat,QLineEdit::text()));     //符号'-'待区分完善
}

//扫描表达式，提取数字串进行进制转换，再拼接
QString ProgrammerExprCalculator::scanAndExec(int previousFormat,int cureentFormat, const QString& text)
{
    m_numvec.clear();
    m_opvec.clear();
    m_textorder = QString();
    QString oldtext = text;
    oldtext.remove(",").remove(" ").replace(QString::fromUtf8("−"),"-");
    for (int i = 0; i < oldtext.length();) {
        if (isNumber(oldtext.at(i))) {
            for (int j = 0; j < oldtext.length() - i; j++) {
                if (i + j == oldtext.length() - 1) {
                    if (isNumber(oldtext.at(i + j))) {
                        m_numvec.append(oldtext.mid(i, j + 1));
                        m_textorder += "0";
                        i += j + 1;
                    } else {
                        m_numvec.append(oldtext.mid(i, j));
                        m_textorder += "0";
                        i += j;
                    }
                    break;
                }
                if (!isNumber(oldtext.at(i + j))) {
                    m_numvec.append(oldtext.mid(i, j));
                    m_textorder += "0";
                    i += j;
                    break;
                }
            }
        } else {
            if (oldtext.at(i).isLower()) {
                if (oldtext.at(i) == 'o') {
                    m_opvec.append(oldtext.mid(i, 2));
                    m_textorder += "1";
                    i += 2;
                } else {
                    m_opvec.append(oldtext.mid(i, 3));
                    m_textorder += "1";
                    i += 3;
                }
            } else if ((i == 0 || !isNumber(oldtext.at(i - 1))) && oldtext.at(i) == QString::fromUtf8("-")    //QString::fromUtf8("－")
                       && oldtext.length() > i + 1 && isNumber(oldtext.at(i + 1))) {
                i++;
                for (int j = 0; j < oldtext.length() - i; j++) {
                    if (!isNumber(oldtext.at(i + j))) {
                        m_numvec.append(oldtext.mid(i - 1, j + 1));
                        m_textorder += "0";
                        i += j;
                        break;
                    }
                    if (i + j == oldtext.length() - 1) {
                        m_numvec.append(oldtext.mid(i - 1, j + 2));
                        m_textorder += "0";
                        i += j + 1;
                        break;
                    }
                }
            } else {
                m_opvec.append(oldtext.at(i));
                m_textorder += "1";
                i++;
            }
        }
    }

    for (int i = 0; i < m_numvec.size(); i++) {

        QString num = AddPrefixForExpr(previousFormat,m_numvec.at(i));
        qDebug() << "AddPrefixForExpr(previousFormat,m_numvec.at(i)):" + num;

        Quantity ans(HNumber(num.toLatin1().data()));
        switch (cureentFormat) {
        case Num_Format_Hex:
            num = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
            qDebug() << "Num_Format_Hex:";
            qDebug() << num;
            break;
        case Num_Format_Oct:
            num = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
            qDebug() << "Num_Format_Oct:";
            qDebug() << num;
            break;
        case Num_Format_Bin:
            num = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
            qDebug() << "Num_Format_Bin:";
            qDebug() << num;
            break;
        default:
            num = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
            qDebug() << "Num_Format_Dec:";
            qDebug() << num;
            break;
        }
        m_numvec.replace(i, num);
    }
    QString newtext = QString();
    for (int i = 0; i < m_textorder.length(); i++) {
        if (m_textorder.at(i) == "0") {
            newtext.append(m_numvec.first());
            m_numvec.pop_front();
        } else {
            newtext.append(m_opvec.first());
            m_opvec.pop_front();
        }
    }
    return newtext.replace("-",QString::fromUtf8("−"));
}

//检测表达式中字符的位数是否超出位数
bool ProgrammerExprCalculator::isNumberOutOfRange(const QString &text)
{
    m_numvec.clear();
    m_opvec.clear();
    m_textorder = QString();

    QString oldtext = text;
    oldtext.remove(",").remove(" ");
    for (int i = 0; i < oldtext.length();) {
        if (isNumber(oldtext.at(i))) {
            for (int j = 0; j < oldtext.length() - i; j++) {
                if (i + j == oldtext.length() - 1) {
                    if (isNumber(oldtext.at(i + j))) {
                        m_numvec.append(oldtext.mid(i, j + 1));
                        m_textorder += "0";
                        i += j + 1;
                    } else {
                        m_numvec.append(oldtext.mid(i, j));
                        m_textorder += "0";
                        i += j;
                    }
                    break;
                }
                if (!isNumber(oldtext.at(i + j))) {
                    m_numvec.append(oldtext.mid(i, j));
                    m_textorder += "0";
                    i += j;
                    break;
                }
            }
        } else {
            if (oldtext.at(i).isLower()) {
               if (oldtext.at(i) == 'o') {
                    m_opvec.append(oldtext.mid(i, 2));
                    m_textorder += "1";
                    i += 2;
                } else {
                    m_opvec.append(oldtext.mid(i, 3));
                    m_textorder += "1";
                    i += 3;
                }
            } else if ((i == 0 || !isNumber(oldtext.at(i - 1))) && oldtext.at(i) == QString::fromUtf8("-")    //QString::fromUtf8("－")
                       && oldtext.length() > i + 1 && isNumber(oldtext.at(i + 1))) {
                i++;
                for (int j = 0; j < oldtext.length() - i; j++) {
                    if (!isNumber(oldtext.at(i + j))) {
                        m_numvec.append(oldtext.mid(i - 1, j + 1));
                        m_textorder += "0";
                        i += j;
                        break;
                    }
                    if (i + j == oldtext.length() - 1) {
                        m_numvec.append(oldtext.mid(i - 1, j + 2));
                        m_textorder += "0";
                        i += j + 1;
                        break;
                    }
                }
            } else {
                m_opvec.append(oldtext.at(i));
                m_textorder += "1";
                i++;
            }
        }
    }
    for (int i = 0; i < m_numvec.size(); i++) {
        QString num = AddPrefixForExpr(m_currentFormat, m_numvec.at(i));
        Quantity ans(HNumber(num.toLatin1().data(), true));
        if (ans.isNan())
            return true;
        // 二进制默认输出最大位数为64位,使用Ncut关闭位数限制
        num = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Binary() +  Quantity::Format::NCut()).remove("0b");

        qDebug() << "*****************isNumberOutOfRange:num******************:";
        qDebug() << num;
        qDebug() << "num.length() --------------------:";
        qDebug() << num.length();
        if (m_currentFormat == Num_Format_Dec) {
            Quantity posans;
            Quantity negans;
            posans = ans - Quantity(HNumber("9223372036854775808"));
            negans = ans + Quantity(HNumber("9223372036854775809"));
            if (!posans.isNegative() || !negans.isPositive())
                return true;
        } else {
            if (num.length() > 64)
            {
                qDebug() << "num.length() > 64:-------------------";
                return true;
            }
        }
    }
    return false;
}

//添加10进制千位符，其他进制用空格分隔,检查位数
void ProgrammerExprCalculator::reformatShowExpr(const QString& text)
{
    qDebug() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
    int oldPosition = this->cursorPosition();

    QString reformatExpr;
    switch (m_currentFormat) {
    case Num_Format_Bin:
        reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" "), 2);
        break;
    case Num_Format_Oct:
        reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" "), 8);
        break;
    case Num_Format_Dec:
        reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" ").remove(","), 10);
//        reformatExpr = Utils::reformatSeparators(QString(text).remove(" ").remove(","));
        break;
    case Num_Format_Hex:
        reformatExpr = Utils::reformatSeparatorsPro(QLineEdit::text().remove(" "), 16);
        break;
    }

    setText(reformatExpr);
    autoZoomFontSize(30);

    // reformat text.
    int oldLength = QString(text).length();
    int newLength = reformatExpr.length();

    int pos;
    if (newLength > oldLength)
        pos = oldPosition + (newLength - oldLength);
    else
        pos = oldPosition - (oldLength - newLength);
//        pos = oldPosition;
    if (pos > newLength)
        pos = newLength;

    this->setCursorPosition(pos);

}

//进制转换计算时，添加进制标准前缀
QString ProgrammerExprCalculator::AddPrefixForExpr(int format,const QString & text)
{
    QString formatText = text;
    QString base = QString();

    switch (format) {
        case Num_Format_Hex:
            base = "0x";
            break;
        case Num_Format_Oct:
            base = "0o";
            break;
        case Num_Format_Bin:
            base = "0b";
            break;
        default:
            break;
        }
        if (base != QString()) {
            for (int i = 0; i < formatText.length();) {
                if ((i == 0) && isNumber(formatText.at(i))) {
                    formatText.insert(i, base);
                    i += 3;
                    continue;
                } else if ((i < formatText.length() - 1) && !isNumber(formatText.at(i)) && isNumber(formatText.at(i + 1))) {
                    formatText.insert(i + 1, base);
                    i += 3;
                    continue;
                }
                i++;
            }
        }
        return formatText;
}


//判断是否为数字和A-F
bool ProgrammerExprCalculator::isNumber(QChar a)
{
    if (a.isDigit() || a == " " || a == "," || AtoF.contains(a))
        return true;
    else
        return false;
}


//手动计算，将10进制结果存入历史记录
void ProgrammerExprCalculator::programmerExprCalc()
{
    QString programmerExpr = m_evaluator->autoFix(QLineEdit::text().remove(" ").remove(",")
                                                  .replace(QString::fromUtf8("−"),"-").replace("÷","\\"));
    qDebug() << "programmerExpr:" + programmerExpr;

    if(programmerExpr.isEmpty())
        return;
    //添加前缀
    QString formatExpr = AddPrefixForExpr(m_currentFormat,programmerExpr);
    qDebug() << "AddPrefixForExpr:";
    qDebug() << formatExpr;

    m_evaluator->setExpression(formatExpr);
    Quantity quantity = m_evaluator->evalUpdateAns();

    if(m_evaluator->error().isEmpty())
    {
        if(!quantity.isNan())
        {
            QJsonObject jsonObject;
            quantity.serialize(jsonObject);
            qInfo() << jsonObject;
            QString messageBin = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed());
            QString messageOct = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed());
            QString messageHex = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed());
            QString messageDec = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
    //        auto messageDec = NumberFormatter::format(quantity);
    //        auto test = NumberFormatter::format()

    //        auto messageDec = DMath::format(quantity, Quantity::Format::Decimal());
    //        emit programmerExprCalcMessageDec(messageDec);
    //        emit programmerExprCalcQuantityDec(quantity);

            switch (m_currentFormat) {
            case Num_Format_Hex:
                emit exprCalcMessageHex(messageHex.remove("0x"));     //去掉前缀
                qDebug() << "formatHex:" + messageHex;
                break;
            case Num_Format_Dec:
                emit exprCalcMessageDec(messageDec);
                emit exprCalcQuantityDec(quantity);
                qDebug() << "formatDec:" + messageDec;
                break;
            case Num_Format_Oct:
                emit exprCalcMessageOct(messageOct.remove("0o"));    //去掉前缀
                qDebug() << "formatOct:" + messageOct;
                break;
            case Num_Format_Bin:
                emit exprCalcMessageBin(messageBin.remove("0b"));    //去掉前缀
                qDebug() << "formatBin:" + messageBin;
                break;
            }
            //将表达式转为10进制后再存入history
            QString exprFormatConvertToDec = scanAndExec(m_currentFormat,NUM_FORMAT_DEC,programmerExpr).replace("\\","÷");
            qDebug() << "exprFormatConvertToDec: " + exprFormatConvertToDec;

            //表达式和结果均以10进制存入历史记录
            m_programmerSession->addHistoryEntry(HistoryEntry(exprFormatConvertToDec,quantity));
            m_programmerHistory = Evaluator::instance()->session()->historyToList();
            m_currentProgrammerHistoryIndex = m_programmerHistory.count();

            emit historyChanged();
            emit calculateMode(Calculation_Mode_Programmer);

            //将十进制表达式和结果进行暂存
            emit stageExprFormat(exprFormatConvertToDec);
            emit stageQuantity(quantity);
            emit stageChanged();
        }
        else
            emit exprCalcNan();
    }
    else
    {
//        emit programmerExprCalcMessageDec(m_evaluator->error());
        emit exprCalcError();
    }

}

//自动计算，结果为10进制，不存入历史记录
void ProgrammerExprCalculator::autoProgrammerExprCalc()
{
    QString programmerExpr = m_evaluator->autoFix(QLineEdit::text().remove(" ").remove(",")
                                                  .replace("−","-").replace("÷","\\"));  //含有括号自动补全

    qDebug() << "programmerExpr:   " +programmerExpr;

    //当超出位数时，限制输入框输入
    if (isNumberOutOfRange(programmerExpr))
    {
        qDebug() << "isNumberOutOfRange:";
        qDebug() << isNumberOutOfRange(programmerExpr);
        QLineEdit::setText(m_tmpPreviousExpr);
        return;
    }else{
        m_tmpPreviousExpr = programmerExpr;
    }

    if(programmerExpr.isEmpty())
    {
        emit programmerExprIsEmpty();
        return ;
    }

    QString formatExpr = AddPrefixForExpr(m_currentFormat,programmerExpr);
    qDebug() << "autoProgrammerExprCalc_____AddPrefixForExpr:";
    qDebug() << formatExpr;

    m_evaluator->setExpression(formatExpr);
    Quantity quantity = m_evaluator->evalUpdateAns();

    if(m_evaluator->error().isEmpty())
    {
        QString formatDec = DMath::format(quantity,  Quantity::Format::Decimal() + Quantity::Format::Fixed());
        int intResult = formatDec.toInt();

        qDebug() << "intResult:";
        qDebug() << intResult;

        emit programmerToNumConversionFormatDec(formatDec);
        emit programmerToNumConversionQuantityDec(quantity);
    }
    else
    {
        qDebug() << "programmerToNumConversionFormatDec___m_evaluator->error():" + m_evaluator->error();
//        emit programmerToNumConversionFormatDec(m_evaluator->error());
        //进制列表不需要具体错误信息
        emit programmerToNumConversionMessageError();
    }
}


void ProgrammerExprCalculator::handleProgrammerFunction_Shift()
{
    if(!m_isShift)
        m_isShift = true;
    else
        m_isShift = false;
    qDebug() << "m_isShift_____:";
    qDebug() << m_isShift;
}

void ProgrammerExprCalculator::handleProgrammerFunction_Lsh()
{
    if(m_isShift)
    {
        if(text().isEmpty())
            handleInsertText("0 rol");
        else
            handleInsertText("rol");
    }
    else
    {
        if(text().isEmpty())
            handleInsertText("0 shl");
        else
            handleInsertText("shl");
    }
}

void ProgrammerExprCalculator::handleProgrammerFunction_Rsh()
{
    if(m_isShift)
    {
        if(text().isEmpty())
            handleInsertText("0 ror");
        else
            handleInsertText("ror");
    }
    else
    {
        if(text().isEmpty())
            handleInsertText("0 shr");
        else
            handleInsertText("shr");
    }
}

void ProgrammerExprCalculator::triggerEnter()
{
    m_currentProgrammerHistoryIndex = m_programmerHistory.count();
    programmerExprCalc();

    emit equalPressed();
    qDebug() << "programmerEqualPressed";
}

void  ProgrammerExprCalculator::setText(const QString& result)
{
    //显示结果中去除'='号
    QString resultModify =  result;
    resultModify.remove('=').replace("-",QString::fromUtf8("−"));
    QLineEdit::setText(resultModify);
}

//程序员模式下取相反数：十进制添负号，其他进制取补码
void ProgrammerExprCalculator::handleProgrammerFunction_Opposite()
{
    if (text().isEmpty()) {
        return;
    }

    QString oldText = text();
    QString exp = text();

    int curPos = cursorPosition();
    if (text() == QString()) {
        setText("(-");
        return;
    }
    if (curPos == 0) {
        return;
    }
    if ((curPos == 0) || curposInNumber(curPos)) {
        return;
    }

    QString sRegNum1 = "[^A-F^0-9,\\s)]";
    QString sRegNum2 = "[A-F0-9,]";
    QRegExp rx1, rx2;
    rx1.setPattern(sRegNum1);
    rx2.setPattern(sRegNum2);
    if (rx1.exactMatch(exp.at(curPos - 1)))
        return;
    else if (exp.at(curPos - 1) == "0" && (curPos <= 1 || !rx2.exactMatch(exp.at(curPos - 2)))) {
        return;
    } else if (curPos > 1 && exp.at(curPos - 1) == " " && exp.at(curPos - 2).isLower()) {
        return;
    } else {
        QString newtext = text();
        int percentpos = cursorPosition();
        int operatorpos =
            newtext.lastIndexOf(QRegularExpression(QStringLiteral("[^A-F^0-9,\\s]")), percentpos - 1);

        bool nooperator = false;
        if (operatorpos < 0) {
            operatorpos++;
            nooperator = true;
        }
        QString exptext;  //表达式
        if (newtext.at(percentpos - 1) == ')') {
            if (operatorpos > 0 && newtext.at(operatorpos - 1) == '(') {
                setText(oldText);
                setCursorPosition(percentpos);
                return;
            }
            do {
                operatorpos = newtext.lastIndexOf('(', operatorpos - 1);
                if (operatorpos <= 0) {
                    break;
                }
            } while (newtext.mid(operatorpos, newtext.size() - operatorpos).count('(') !=
                     newtext.mid(operatorpos, percentpos - operatorpos).count(')'));

            //匹配到的(不在开头且(左侧是字母
            QString sRegNum3 = "[a-z]";
            QRegExp latterrx;
            latterrx.setPattern(sRegNum3);
            if (operatorpos > 0 && latterrx.exactMatch(text().at(operatorpos - 1))) {
                int funpos = -1; //记录函数位
                int i;
                for (i = 0; i < m_funclist.size(); i++) {
                    //记录(左侧离(最近的函数位
                    funpos = text().lastIndexOf(m_funclist[i], operatorpos - 1);
                    if (funpos != -1 && (funpos + m_funclist[i].length() == operatorpos))
                        break; //(在函数结尾
                    else
                        funpos = -1;
                }
                if (funpos != -1)
                    operatorpos = operatorpos - m_funclist[i].length(); //截取函数
            }

            exptext = newtext.mid(operatorpos,
                                  percentpos - operatorpos);  //截取表达式
        } else {
            exptext = newtext.mid(operatorpos + (nooperator == true ? 0 : 1),
                                  percentpos - operatorpos + (nooperator == true ? 1 : 0) - 1);
            //截取表达式
        }
        if (exptext.count("(") == exptext.count(")")) {
            setCursorPosition(curPos - exptext.length());
            if (m_currentFormat == Num_Format_Dec) {
                insert("(-");
                int afterinsertpos = cursorPosition();
                setCursorPosition(afterinsertpos + exptext.length());
                insert(")");
            } else {
                QString basetag = QString();
                BASE_TAG(basetag);
                m_evaluator->setExpression("0-" + basetag + exptext);
                Quantity ans = m_evaluator->evalUpdateAns();
                if (m_evaluator->error().isEmpty()) {
                    if (ans.isNan() && !m_evaluator->isUserFunctionAssign())
                        return;
                    QString result;
                    switch (m_currentFormat) {
                    case Num_Format_Hex:
                        result = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
                        result = Utils::formatThousandsSeparatorsPro(result, 16);
                        break;
                    case Num_Format_Oct:
                        result = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
                        result = Utils::formatThousandsSeparatorsPro(result, 8);
                        break;
                    case Num_Format_Bin:
                        result = DMath::format(ans, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
                        result = Utils::formatThousandsSeparatorsPro(result, 2);
                        break;
                    default:
                        return;
                    }
                    QString text = QLineEdit::text();
                    text.remove(curPos - exptext.length(), exptext.length());
                    text.insert(curPos - exptext.length(), result);
                    setText(text);
                    setCursorPosition(curPos - exptext.length() + result.length());
                } else {
                    return;
                }
            }
        }
    }

}
bool ProgrammerExprCalculator::curposInNumber(int curpos)
{
    if (text().length() <= curpos) {
        return false;
    }
    if (isNumber(text().at(curpos))) {
        if (curpos < text().length() - 1
                && text().at(curpos) == " " && !isNumber(text().at(curpos + 1))) {
            return false;
        } else {
            return true;
        }
    }
    return false;
}

bool ProgrammerExprCalculator::judgeInsertPos()
{
    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);
    if (cursorPosition() > 0 && rx.exactMatch(text().at(cursorPosition() - 1))) {
        for (int i = 0; i < m_funclist.size(); i++) {
            //记录光标左侧离光标最近的函数位
            int funpos = text().lastIndexOf(m_funclist[i], cursorPosition() - 1);
            if (funpos != -1 && (funpos < cursorPosition()) && (cursorPosition() < funpos + m_funclist[i].length()))
                return false; //光标在函数中
        }
    }
    return true;
}


//删除逗号和选中后删除 ，导致光标位置移动，有待进一步修改
void ProgrammerExprCalculator::handleProgrammerFunction_Backspace()
{
    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);

    if((!selectedText().isEmpty())&& !(selectedText() == QLineEdit::text())) {
        deselect(); //取消选中
        return;
    } else {
        QString text = QLineEdit::text();
        int cur = QLineEdit::cursorPosition();
        int funpos = -1;
        int i;
        int Sepold = text.count(",") + text.count(" ");
        if (text.size() > 0 && cur > 0 && (text[cur - 1] == "," || (text[cur - 1] == " " && !text[cur - 2].isLower()))) {
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
                    int Sepnew = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
                    QLineEdit::setCursorPosition(funpos + Sepnew - Sepold + 1);
                }
            } else if (QLineEdit::cursorPosition() > 1 && (text[cur - 1] == " " && text[cur - 2].isLower())) {
                for (i = 0; i < m_funclist.size(); i++) {
                    //记录光标左侧离光标最近的函数位
                    funpos = QLineEdit::text().lastIndexOf(m_funclist[i], QLineEdit::cursorPosition() - 2);
                    if (funpos != -1 && (funpos <= QLineEdit::cursorPosition())
                            && (QLineEdit::cursorPosition() <= funpos + m_funclist[i].length() + 1))
                        break; //光标在函数开头和函数结尾之间
                    else
                        funpos = -1;
                }
                if (funpos != -1) {
                    QLineEdit::setText(QLineEdit::text().remove(funpos, m_funclist[i].length()));
                    int Sepnew = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
                    QLineEdit::setCursorPosition(funpos + Sepnew - Sepold + 1);
                }
            } else {
                int proNumber = text.count(",") + text.count(" ");
                QLineEdit::backspace();
                int separator = proNumber - QLineEdit::text().count(",") - QLineEdit::text().count(" ");
                int newPro = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
                if (cur > 0) {
                    QString sRegNum1 = "[0-9A-F]+";
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
                }
            }
        }
    }

}


//限制键盘输入
void ProgrammerExprCalculator::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();

    qInfo() << "keyPressed:" << event;

    switch (key) {
    case Qt::Key_0:
        handleInsertText("0");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_0);
        break;
    case Qt::Key_1:
        handleInsertText("1");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_1);
        break;
    case Qt::Key_2:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("2");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_2);
        break;
    case Qt::Key_3:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("3");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_3);
        break;
    case Qt::Key_4:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("4");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_4);
        break;
    case Qt::Key_5:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("5");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_5);
        break;
    case Qt::Key_6:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("6");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_6);
        break;
    case Qt::Key_7:
        if(m_currentFormat == Num_Format_Bin)
            break;
        handleInsertText("7");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_7);
        break;
    case Qt::Key_8:
        if(m_currentFormat == Num_Format_Bin || m_currentFormat == Num_Format_Oct)
            break;
        handleInsertText("8");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_8);
        break;
    case Qt::Key_9:
        if(m_currentFormat == Num_Format_Bin || m_currentFormat == Num_Format_Oct)
            break;
        handleInsertText("9");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_9);
        break;
    case Qt::Key_A:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("A");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_A);
        }
         break;
    case Qt::Key_B:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("B");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_B);
        }
        break;
    case Qt::Key_C:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("C");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_C);
        }
        break;
    case Qt::Key_D:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("D");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_D);
        }
        break;
    case Qt::Key_E:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("E");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_E);
        }
        break;
    case Qt::Key_F:
        if(m_currentFormat == Num_Format_Hex){
            handleInsertText("F");
            m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_F);
        }
        break;
    case Qt::Key_Plus:
        handleInsertText("+");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Add);
        break;
    case Qt::Key_Minus:
//        insert("-"); //deepin   显示比较短
        handleInsertText("−"); //speedcrush  显示比较长
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Sub);
        break;

    case Qt::Key_Asterisk:
        handleInsertText("×");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Mult);
        break;
    case Qt::Key_Slash:
        handleInsertText("÷");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Divide);
        break;
    case Qt::Key_ParenLeft:
        handleInsertText("(");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_LeftBracket);
        break;
    case Qt::Key_ParenRight:
        handleInsertText(")");
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_RightBracket);
        break;
    case Qt::Key_Enter:
        programmerExprCalc();
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Return:
        programmerExprCalc();
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Equal:
        programmerExprCalc();
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Equal);
        break;
    case Qt::Key_Backspace:
        handleProgrammerFunction_Backspace();
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Backspace);
        break;
    case Qt::Key_Delete:
        clear();
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Clear);
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
        m_programmerKeysPage->handleProgrammerButtonAnimate(Button_Key_Clear);
        break;
    }
//    QLineEdit::keyPressEvent(event);
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
