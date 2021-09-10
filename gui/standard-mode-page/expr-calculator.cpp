#include "expr-calculator.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/sessionhistory.h"
#include "utils.h"

//#include <QHeaderView>
#include <QRegularExpression>
#include <QDebug>

ExprCalculator::ExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    connect(this,SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this,SIGNAL(exprCalcMessageDec(const QString &)), this, SLOT(setText(const QString &)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));

    connect(this,SIGNAL(selectionChanged()), this, SLOT(disableSelectText()));
//    m_funclist = {"lg", "ln", "log","sqrt","%"};
    m_funclist = {"lg", "ln", "log","sqrt"};


}

void ExprCalculator::setSession(Session *session)
{
    m_standardSession = session;
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_standardSession);
}

void ExprCalculator::disableSelectText()
{
    qobject_cast<QLineEdit*>(sender())->deselect();
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
//        pos = oldPosition;
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
//        auto messageDec = tr("%1").arg(formatDec);

        emit exprCalcMessageDec(formatDec);
        emit exprCalcQuantityDec(quantity);
        qDebug() << formatDec;
        qDebug() << "formatDec:"+formatDec;
        //存入标准历史记录
        m_standardSession->addHistoryEntry(HistoryEntry(expr,quantity));
        emit standardHistoryChanged();
        //存入暂存记录
        emit standardToStageExprFormat(expr);
        emit standardToStageQuantity(quantity);
        emit standardStageChanged();

    } else
    {
//        emit exprCalcMessageDec(m_evaluator->error());
        emit exprCalcError();
    }

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
    case Qt::Key_Percent: insert("%"); break;
    case Qt::Key_AsciiCircum:
        if(text().isEmpty())
            insert("0^");
        else
            insert("^");
        break;
    case Qt::Key_At:handleFunction_Sqrt();break;
    case Qt::Key_Q:handleFunction_Square(); break;
    case Qt::Key_R:handleFunction_Reciprocal(); break;
    case Qt::Key_Enter:exprCalc(); break;
    case Qt::Key_Return:exprCalc(); break;
    case Qt::Key_Equal:exprCalc(); break;
    case Qt::Key_Backspace:handleFunction_Backspace(); break;
    case Qt::Key_Delete:clear(); break;
    case Qt::Key_Left:
        setCursorPosition(cursorPosition() - 1);
        break;
    case Qt::Key_Right:
        setCursorPosition(cursorPosition() + 1);
        break;
    case Qt::Key_Escape: clear(); break;
    }
}

//重写双击事件，防止双击时触发光标选中
void ExprCalculator::mouseDoubleClickEvent(QMouseEvent *event)
{
    return;
}

void ExprCalculator::handleFunction_Sqrt()
{
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);

    if(tmp.right(1) >= '0' && tmp.right(1)<= '9')
        insert("×sqrt()");
    else
        insert("sqrt()");
    cursorBackward(false);
}

void ExprCalculator::handleFunction_Square()
{
    if(text().isEmpty())
        insert("0^2");
    else
        insert("^2");
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


void ExprCalculator::handleFunction_Backspace()
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
