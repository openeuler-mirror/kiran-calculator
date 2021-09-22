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
#include <QDebug>
#include <QRegularExpression>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

bool ScienceExprCalculator::m_FE = false;
bool ScienceExprCalculator::m_isShift = false;
bool ScienceExprCalculator::m_isHYP = false;
ScienceExprCalculator::ScienceExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    Settings::instance()->angleUnit = 'd';
    Settings::instance()->complexNumbers = false;
    DMath::complexMode = false;                    //关闭复数模式
    m_evaluator = Evaluator::instance();

    connect(this,&ScienceExprCalculator::returnPressed,this,&ScienceExprCalculator::triggerEnter);
    connect(this, SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this, SIGNAL(scienceExprCalcMessage(const QString&)), this, SLOT(setText(const QString&)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));

    //    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));

    m_funclist = {"arcsin", "arccos", "arctan","arsinh","arcosh","artanh",
                  "sin", "cos", "tan", "sinh","cosh","tanh",
                  "lg", "ln", "log", "mod", "sqrt", "yroot", "pi", "e",
                 };

    initMenuAndAction();
    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));
}

void ScienceExprCalculator::setSession(Session *session)
{
    m_scienceSession = session;
//    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_scienceSession);
    m_evaluator->initializeBuiltInVariables();
}

void ScienceExprCalculator::handleInsertText(const QString &text)
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

void ScienceExprCalculator::disableSelectText()
{
    qobject_cast<QLineEdit*>(sender())->deselect();
}

void ScienceExprCalculator::showMenu(const QPoint &point)
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

void ScienceExprCalculator::copyResultToClipboard()
{
    if(text().isEmpty())
        return;
    QApplication::clipboard()->setText(selectedText());
}

void ScienceExprCalculator::paste()
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
    QStringList list = text.split(QRegExp("[0-9+−×÷/()%^!e.,。]"));
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

void ScienceExprCalculator::exprSelectAll()
{
    selectAll();
}

void ScienceExprCalculator::scienceExprCalc()
{
    QString scienceExpr = m_evaluator->autoFix(QLineEdit::text().remove(" ").remove(","));

    if(scienceExpr.isEmpty())
        return;

    m_evaluator->setExpression(scienceExpr);
    Quantity quantity = m_evaluator->evalUpdateAns();
    QString  message;
    if(m_evaluator->error().isEmpty())
    {
        if(!quantity.isNan())
        {
            if(m_FE)
                message= DMath::format(quantity, Quantity::Format::Scientific());
            else
                message = NumberFormatter::format(quantity);

            emit scienceExprCalcMessage(message);
            emit scienceExprCalcQuantity(quantity);

            qDebug() << "science message";
            qDebug() << message;

            m_scienceSession->addHistoryEntry(HistoryEntry(scienceExpr,quantity));
            emit scienceHistoryChanged();

            emit scienceToStageExprFormat(scienceExpr);
            emit scienceToStageQuantity(quantity);
            emit scienceStageChanged();
        }
        else
            emit scienceExprCalcNan();
    }
    else
    {
//        emit scienceExprCalcMessage(m_evaluator->error());
        emit scienceExprCalcError();
    }

}

void ScienceExprCalculator::insert(const QString& text)
{
    QLineEdit::insert(text);
}

void ScienceExprCalculator::triggerEnter()
{
    scienceExprCalc();
    emit scienceEqualPressed();
    qDebug() << "scienceEqualPressed()";
}


//添加10进制千位符
void ScienceExprCalculator::reformatShowExpr(const QString& text)
{
    int oldPosition = this->cursorPosition();

    QString reformatExpr;
    reformatExpr = Utils::reformatSeparators(QString(text).remove(" ").remove(","));
//    reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" ").remove(","),10);


    setText(reformatExpr);

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

void ScienceExprCalculator::setText(const QString& result)
{
    QString resultModify = result;
    resultModify.remove("=");
    QLineEdit::setText(resultModify);
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

//取反功能有问题，考虑暂时不修改了，或者延后再做   例如：555 + sqrt(55)+44   5%
void ScienceExprCalculator::handleFunction_Opposite()
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

bool ScienceExprCalculator::expressionInFunc(QString &text)
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
            newtext.lastIndexOf(QRegularExpression(QStringLiteral("[^0-9,.eπE]")), percentpos - 1);

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

void ScienceExprCalculator::handleFunction_Backspace()
{
    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);

    QString text = QLineEdit::text();
    int cur = QLineEdit::cursorPosition();
    int funpos = -1;
    int i;
    if((!selectedText().isEmpty())&& !(selectedText() == QLineEdit::text()))
    {
        qDebug() << "选中时不可删除（暂定）";
        deselect(); //取消选中
        return;
    }
    if (text.size() > 0 && cur > 0 && text[cur - 1] == ",") {
        text.remove(cur - 2, 2);
        QLineEdit::setText(text);
        // 20200401 symbolFaultTolerance
        //                QLineEdit::setText(QLineEdit::symbolFaultTolerance(QLineEdit::text()));
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
            // 20200401 symbolFaultTolerance
            //                    QLineEdit::setText(QLineEdit::symbolFaultTolerance(QLineEdit::text()));

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

//void ScienceExprCalculator::handleFunction_Backspace()
//{
//    QString sRegNum = "[a-z]";
//    QRegExp rx;
//    rx.setPattern(sRegNum);

//    QString text = QLineEdit::text();
//    int cur = QLineEdit::cursorPosition();
//    int funpos = -1;
//    int i;
//    int Sepold = text.count(",") + text.count(" ");
//    if (text.size() > 0 && cur > 0 && (text[cur - 1] == "," || (text[cur - 1] == " " && !text[cur - 2].isLower()))) {
//        text.remove(cur - 2, 2);
//        QLineEdit::setText(text);
//        QLineEdit::setCursorPosition(cur - 2);
//    } else {
//        //退函数
//        //光标不在开头且光标左侧是字母
//        if (QLineEdit::cursorPosition() > 0 && rx.exactMatch(QLineEdit::text().at(QLineEdit::cursorPosition() - 1))) {
//            for (i = 0; i < m_funclist.size(); i++) {
//                //记录光标左侧离光标最近的函数位
//                funpos = QLineEdit::text().lastIndexOf(m_funclist[i], QLineEdit::cursorPosition() - 1);
//                if (funpos != -1 && (funpos <= QLineEdit::cursorPosition())
//                        && (QLineEdit::cursorPosition() <= funpos + m_funclist[i].length()))
//                    break; //光标在函数开头和函数结尾之间
//                else
//                    funpos = -1;
//            }
//            if (funpos != -1) {
//                QLineEdit::setText(QLineEdit::text().remove(funpos, m_funclist[i].length()));
//                int Sepnew = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
//                QLineEdit::setCursorPosition(funpos + Sepnew - Sepold + 1);
//            }
//        } else if (QLineEdit::cursorPosition() > 1 && (text[cur - 1] == " " && text[cur - 2].isLower())) {
//            for (i = 0; i < m_funclist.size(); i++) {
//                //记录光标左侧离光标最近的函数位
//                funpos = QLineEdit::text().lastIndexOf(m_funclist[i], QLineEdit::cursorPosition() - 2);
//                if (funpos != -1 && (funpos <= QLineEdit::cursorPosition())
//                        && (QLineEdit::cursorPosition() <= funpos + m_funclist[i].length() + 1))
//                    break; //光标在函数开头和函数结尾之间
//                else
//                    funpos = -1;
//            }
//            if (funpos != -1) {
//                QLineEdit::setText(QLineEdit::text().remove(funpos, m_funclist[i].length()));
//                int Sepnew = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
//                QLineEdit::setCursorPosition(funpos + Sepnew - Sepold + 1);
//            }
//        } else {
//            int proNumber = text.count(",") + text.count(" ");
//            QLineEdit::backspace();
//            int separator = proNumber - QLineEdit::text().count(",") - QLineEdit::text().count(" ");
//            int newPro = QLineEdit::text().count(",") + QLineEdit::text().count(" ");
//            if (cur > 0) {
//                QString sRegNum1 = "[0-9A-F]+";
//                QRegExp rx1;
//                rx1.setPattern(sRegNum1);
//                //退数字
//                if (rx1.exactMatch(text.at(cur - 1)) && proNumber > newPro) {
//                    if (text.mid(cur, text.length() - cur) == QLineEdit::text().mid(QLineEdit::text().length() - (text.length() - cur), text.length() - cur)) {
//                        QLineEdit::setCursorPosition(cur - 2);
//                    } else
//                        QLineEdit::setCursorPosition(cur - 1);
//                } else {
//                    if (separator < 0) {
//                        QLineEdit::setCursorPosition(cur - 1 - separator);
//                    } else {
//                        QLineEdit::setCursorPosition(cur - 1);
//                    }
//                }
//            }
//        }
//    }

//}



void ScienceExprCalculator::handleFunction_Exp()
{
    //        ui->scienceExprEdit->insert("exp()");
    //        ui->scienceExprEdit->cursorBackward(false);

    //科学计数法，不是指数函数
    if(text().isEmpty())
        insert(QLatin1String("0.e+"));
    else
    {
        if(text().contains("."))
//            insert(QLatin1String("e+"));
            handleInsertText("e+");

        else
//            insert(QLatin1String(".e+"));
            handleInsertText(".e+");
    }
}

void ScienceExprCalculator::handleFunction_Reciprocal()
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

void ScienceExprCalculator::handleFunction_Factorials()
{
    if(text().isEmpty())
        handleInsertText("0!");
    else
        handleInsertText("!");
}

bool ScienceExprCalculator::judgeInsertPos()
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

void ScienceExprCalculator::initMenuAndAction()
{
    m_menu = new QMenu(this);
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
    qDebug() << "m_FE________:";
    qDebug() << m_FE;
}

void ScienceExprCalculator::handleFunction_Shift()
{
    if(!m_isShift)
        m_isShift = true;
    else
        m_isShift = false;
    qDebug() << "m_isShift_____:";
    qDebug() << m_isShift;
}

void ScienceExprCalculator::handleFunction_HYP()
{
    if(!m_isHYP)
        m_isHYP = true;
    else
        m_isHYP = false;
    qDebug() << "m_isHYP___:";
    qDebug() << m_isHYP;
}


void ScienceExprCalculator::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();

    switch (key) {
    case Qt::Key_0: handleInsertText("0"); break;
    case Qt::Key_1: handleInsertText("1"); break;
    case Qt::Key_2: handleInsertText("2"); break;
    case Qt::Key_3: handleInsertText("3"); break;
    case Qt::Key_4: handleInsertText("4"); break;
    case Qt::Key_5: handleInsertText("5"); break;
    case Qt::Key_6: handleInsertText("6"); break;
    case Qt::Key_7: handleInsertText("7"); break;
    case Qt::Key_8: handleInsertText("8"); break;
    case Qt::Key_9: handleInsertText("9"); break;

    case Qt::Key_Plus: handleInsertText("+"); break;
    case Qt::Key_Minus: handleInsertText("−"); break;
    case Qt::Key_Asterisk: handleInsertText("×"); break;
    case Qt::Key_Slash: handleInsertText("÷"); break;
    case Qt::Key_Period: handleInsertText("."); break;
    case Qt::Key_Percent: handleInsertText("%"); break;
    case Qt::Key_E:
//        insert(QLatin1String("e"));
        handleFunction_Exp();
        break;
    case Qt::Key_P:
        handleInsertText(QLatin1String("pi"));
        break;
    case Qt::Key_Q:handleFunction_Square(); break;
    case Qt::Key_R:handleFunction_Reciprocal(); break;
    case Qt::Key_T:handleFunction_Tan(); break;
    case Qt::Key_S:handleFunction_Sin(); break;
    case Qt::Key_O:handleFunction_Cos(); break;
    case Qt::Key_L:handleFunction_Log(); break;
    case Qt::Key_Y:
        if(text().isEmpty())
            handleInsertText("0^");
        else
            handleInsertText("^");
        break;
    case Qt::Key_AsciiCircum:
        if(text().isEmpty())
            handleInsertText("0^");
        else
            handleInsertText("^");
        break;
    case Qt::Key_At:
        handleInsertText("sqrt()");
        cursorBackward(false);
        break;
    case Qt::Key_Exclam:handleFunction_Factorials();break;
    case Qt::Key_Enter:
        scienceExprCalc();
        break;
    case Qt::Key_Return:
        scienceExprCalc();
        break;
    case Qt::Key_Equal:
        scienceExprCalc();
        break;
    case Qt::Key_Backspace:
        handleFunction_Backspace();
        break;
    case Qt::Key_Delete:
        clear();
        break;
    case Qt::Key_Left:
        setCursorPosition(cursorPosition() - 1);
        break;
    case Qt::Key_Right:
        setCursorPosition(cursorPosition() + 1);
        break;
    case Qt::Key_Escape: clear(); break;
    }

    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C))
    {
        copyResultToClipboard();
    }
    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_V))
    {
        paste();
    }
    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_A))
    {
        exprSelectAll();
    }

}
