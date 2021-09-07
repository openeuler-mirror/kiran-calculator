#include "expr-calculator.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/sessionhistory.h"
#include "utils.h"

#include <QHeaderView>
#include <QDebug>

ExprCalculator::ExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    connect(this,SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this,SIGNAL(exprCalcMessageDec(const QString &)), this, SLOT(setText(const QString &)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));
}

void ExprCalculator::setSession(Session *session)
{
    m_standardSession = session;
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_standardSession);
}

//添加10进制千位符
void ExprCalculator::reformatShowExpr(const QString& text)
{
    int oldPosition = this->cursorPosition();

    QString reformatExpr;
    reformatExpr = Utils::reformatSeparators(QString(text).remove(" ").remove(","));
    setText(reformatExpr);

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

//传入QLineEdit::text()
//Evaluator类中包含运算规则和计算，在该类外部只做括号匹配检测
//内部没进行括号匹配，只进行将括号去除的操作
void ExprCalculator::exprCalc()
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
        //输出结果的进制转换
        //十进制
        auto formatDec = NumberFormatter::format(quantity);
        auto messageDec = tr("%1").arg(formatDec);

        emit exprCalcMessageDec(messageDec);
        emit exprCalcQuantityDec(quantity);
        qDebug() << messageDec;
        qDebug() << "formatDec:"+formatDec;
        //存入标准历史记录
        m_standardSession->addHistoryEntry(HistoryEntry(expr,quantity));
        emit standardHistoryChanged();
        //存入暂存记录
        emit standardToStageExprFormat(expr);
        emit standardToStageQuantity(quantity);
        emit standardStageChanged();


    } else
        emit exprCalcMessageDec(m_evaluator->error());
}

void ExprCalculator::insert(const QString& text)
{
    QLineEdit::insert(text);
}

//按回车 和 (=号)  发射信号.进行计算
void ExprCalculator::triggerEnter()
{
    exprCalc();
    emit equalPressed();
    qDebug() << "returnPressed";
}

void  ExprCalculator::setText(const QString& result)
{
    //显示结果中去除'='号
    QString resultModify =  result;
    resultModify.remove('=');
    QLineEdit::setText(resultModify);
}

void ExprCalculator::evaluate()
{
    triggerEnter();
}



void ExprCalculator::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();
    qInfo() << "keyPressed:" << event;

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

    case Qt::Key_Enter:
        exprCalc();
        break;
    case Qt::Key_Return:
        exprCalc();
        break;
    case Qt::Key_Equal:
        exprCalc();
        break;
    case Qt::Key_Backspace:
        backspace();
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

void ExprCalculator::handleFunction_Sqrt()
{
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);
    QString tmp_2 = tmp.right(1);

    if(tmp_2 >= '0' && tmp_2<= '9')
        insert("×sqrt()");
    else
        insert("sqrt()");
}

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
