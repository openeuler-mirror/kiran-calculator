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

bool ScienceExprCalculator::m_FE = false;
bool ScienceExprCalculator::m_isShift = false;
bool ScienceExprCalculator::m_isHYP = false;
ScienceExprCalculator::ScienceExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    Settings::instance()->angleUnit = 'd';
    Settings::instance()->complexNumbers = false;
    DMath::complexMode = false;                    //关闭复数模式
    m_evaluator = Evaluator::instance();

    connect(this, SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this, SIGNAL(scienceExprCalcMessage(const QString&)), this, SLOT(setText(const QString&)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));
    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));

    m_funclist = {"arcsin", "arccos", "arctan","arsinh","arcosh","artanh",
                  "sin", "cos", "tan", "sinh","cosh","tanh",
                  "lg", "ln", "log", "mod", "sqrt", "yroot", "pi", "e"
                 };
}

void ScienceExprCalculator::setSession(Session *session)
{
    m_scienceSession = session;
//    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_scienceSession);
    m_evaluator->initializeBuiltInVariables();
}

void ScienceExprCalculator::disableSelectText()
{
    qobject_cast<QLineEdit*>(sender())->deselect();
}

void ScienceExprCalculator::mouseDoubleClickEvent(QMouseEvent *)
{
    return;
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
    reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" ").remove(","), 10);

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
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);

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
        if(tmp.right(1) >= '0' && tmp.right(1)<= '9')
            insert("×sqrt()");
        else
            insert("sqrt()");
    }

}

void ScienceExprCalculator::handleFunction_Square()
{
    if(m_isShift)
    {
        if(text().isEmpty())
            insert("0^3");
        else
            insert("^3");
    }
    else
    {
        if(text().isEmpty())
            insert("0^2");
        else
            insert("^2");
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
        insert("arsinh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        insert("arcsin()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        insert("sinh()");
        cursorBackward(false);
    }
    else
    {
        insert("sin()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Cos()
{

    if(m_isShift && m_isHYP)
    {
        insert("arcosh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        insert("arccos()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        insert("cosh()");
        cursorBackward(false);
    }
    else
    {
        insert("cos()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Tan()
{
    if(m_isShift && m_isHYP)
    {
        insert("artanh()");
        cursorBackward(false);
    }
    else if(m_isShift && !m_isHYP)
    {
        insert("arctan()");
        cursorBackward(false);
    }
    else if(!m_isShift && m_isHYP)
    {
        insert("tanh()");
        cursorBackward(false);
    }
    else
    {
        insert("tan()");
        cursorBackward(false);
    }
}

void ScienceExprCalculator::handleFunction_Log()
{
    if(m_isShift)
    {
        insert("ln()");
         cursorBackward(false);
    }
    else
    {
        insert("lg()");
        cursorBackward(false);
    }
}


void ScienceExprCalculator::handleFunction_10n()
{
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);
    QString tmp_2 = tmp.right(1);

    if(m_isShift)
    {
        if(tmp_2 >= '0' && tmp_2<= '9')
            insert("×e^");
        else
            insert("e^");
    }
    else
    {
        if(tmp_2 >= '0' && tmp_2<= '9')
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

    qDebug() << "expr.left(pos):";
    qDebug() << tmp;

    QString tmp_2 = tmp.right(1);
    qDebug() << "expr.right(1):";
    qDebug() << tmp_2;

    if(tmp_2 >= '0' && tmp_2<= '9')
        insert("×3.14159265358979323846");
    else
        insert("3.14159265358979323846");
}
/*
void ScienceExprCalculator::handleFunction_Backspace()
{
//    int currentPos = cursorPosition();
//    QString expr = text();
//    if(currentPos !=0 && (expr.at(currentPos - 1) == ","))
//    {
//        setCursorPosition(currentPos - 1);

//        qDebug() << "cursorPosition: ";
//        qDebug() << cursorPosition();

//        backspace();
//    }
//    else
//        backspace();

    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);

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
*/

void ScienceExprCalculator::handleFunction_Backspace()
{
    QString sRegNum = "[a-z]";
    QRegExp rx;
    rx.setPattern(sRegNum);
    SSelection selection = getSelection();
    if (!selection.selected.isEmpty()) {
        QString text = QLineEdit::text();
        QString seloldtext = text;
        int removepos = 0; //被删除位置
        //光标不在开头且光标左侧是字母或者光标右侧是字母
        if ((selection.curpos > 0 &&
                rx.exactMatch(QLineEdit::text().at(selection.curpos - 1)))
                || (selection.curpos + selection.selected.size() < QLineEdit::text().size() && rx.exactMatch(QLineEdit::text().at(selection.curpos + selection.selected.size())))) {
            int selleftfunlen = 0; //选中左侧一部分函数长度
            int funpos = -1;
            int rightfunpos = -1;
            int j;
            for (int i = 0; i < m_funclist.size(); i++) {
                //记录光标左侧离光标最近的函数位
                funpos = QLineEdit::text().lastIndexOf(m_funclist[i], selection.curpos - 1);
                if (funpos != -1 && (funpos <= selection.curpos) && (selection.curpos < funpos + m_funclist[i].length())) {
                    selleftfunlen = selection.curpos - funpos;
                    break; //光标在函数开头和函数结尾之间
                } else
                    funpos = -1;
            }
            for (j = 0; j < m_funclist.size(); j++) {
                //记录离光标最近的右侧函数位
                rightfunpos = QLineEdit::text().lastIndexOf(m_funclist[j], selection.curpos + selection.selected.size() - 1);
                if (rightfunpos != -1 && (rightfunpos + m_funclist[j].length() > selection.curpos + selection.selected.size()))
                    break;
                else
                    rightfunpos = -1;
            }
            if (funpos != -1 || rightfunpos != -1) {
                if (funpos != -1 && rightfunpos == -1) {
                    removepos = funpos;
                    text.remove(funpos, selection.selected.size() + selleftfunlen); //仅左侧有函数
                } else if (rightfunpos != -1 && funpos == -1) {
                    removepos = selection.curpos;
                    text.remove(selection.curpos, rightfunpos + m_funclist[j].length() - selection.curpos); //仅右侧有函数
                } else {
                    removepos = funpos;
                    text.remove(funpos, rightfunpos + m_funclist[j].length() - funpos); //函数中或左右均有
                }
            } else {
                removepos = selection.curpos;
                text.remove(selection.curpos, selection.selected.size());
            }
        } else {
            removepos = selection.curpos;
            text.remove(selection.curpos, selection.selected.size());
        }

        QLineEdit::setText(text); //输入栏删除被选中
        // 20200401 symbolFaultTolerance
//            QLineEdit::setText(QLineEdit::symbolFaultTolerance(QLineEdit::text()));

        // 20200316选中部分光标置位问题修复
        if ((seloldtext.mid(0, removepos).remove(QRegExp("[＋－×÷/,.%()E]")).length()) ==
                QLineEdit::text().mid(0, removepos).remove(QRegExp("[＋－×÷/,.%()E]")).length())
            QLineEdit::setCursorPosition(removepos);
        else if ((seloldtext.mid(0, removepos).remove(QRegExp("[＋－×÷/,.%()E]")).length()) >
                 QLineEdit::text().mid(0, removepos).remove(QRegExp("[＋－×÷/,.%()E]")).length())
            QLineEdit::setCursorPosition(removepos + 1);
        else
            QLineEdit::setCursorPosition(removepos - 1);
    } else {
        QString text = QLineEdit::text();
        int cur = QLineEdit::cursorPosition();
        int funpos = -1;
        int i;
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
}

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
            insert(QLatin1String("e+"));
        else
            insert(QLatin1String(".e+"));
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
        insert("0!");
    else
        insert("!");
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
    case Qt::Key_0: insert("0"); break;
    case Qt::Key_1: insert("1"); break;
    case Qt::Key_2: insert("2"); break;
    case Qt::Key_3: insert("3"); break;
    case Qt::Key_4: insert("4"); break;
    case Qt::Key_5: insert("5"); break;
    case Qt::Key_6: insert("6"); break;
    case Qt::Key_7: insert("7"); break;
    case Qt::Key_8: insert("8"); break;
    case Qt::Key_9: insert("9"); break;

    case Qt::Key_Plus: insert("+"); break;
    case Qt::Key_Minus: insert("−"); break;
    case Qt::Key_Asterisk: insert("×"); break; //*
    case Qt::Key_Slash: insert("÷"); break; // "/"
    case Qt::Key_Period: insert("."); break;
    case Qt::Key_Percent: insert("%"); break;
    case Qt::Key_E:
//        insert(QLatin1String("e"));
        handleFunction_Exp();
        break;
    case Qt::Key_P:
        insert(QLatin1String("pi"));
        break;
    case Qt::Key_Q:handleFunction_Square(); break;
    case Qt::Key_R:handleFunction_Reciprocal(); break;
    case Qt::Key_T:handleFunction_Tan(); break;
    case Qt::Key_S:handleFunction_Sin(); break;
    case Qt::Key_O:handleFunction_Cos(); break;
    case Qt::Key_L:handleFunction_Log(); break;
    case Qt::Key_Y:
        if(text().isEmpty())
            insert("0^");
        else
            insert("^");
        break;
    case Qt::Key_AsciiCircum:
        if(text().isEmpty())
            insert("0^");
        else
            insert("^");
        break;
    case Qt::Key_At:
        insert("sqrt()");
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
}
