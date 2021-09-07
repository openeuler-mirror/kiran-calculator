#include "science-expr-calculator.h"
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/sessionhistory.h"
#include "utils.h"
#include <QDebug>

bool ScienceExprCalculator::m_FE = false;
ScienceExprCalculator::ScienceExprCalculator(QWidget* parent) : QLineEdit(parent)
{
    Settings::instance()->angleUnit = 'd';
    connect(this, SIGNAL(returnPressed()), this, SLOT(triggerEnter()));
    connect(this, SIGNAL(scienceExprCalcMessage(const QString&)), this, SLOT(setText(const QString&)));
    connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(reformatShowExpr(const QString&)));
}

void ScienceExprCalculator::setSession(Session *session)
{
    m_scienceSession = session;
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_scienceSession);
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
        emit scienceExprCalcMessage(m_evaluator->error());
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
//    reformatExpr = Utils::reformatSeparatorsPro(QString(text).remove(" ").remove(","), 10);
    reformatExpr = Utils::reformatSeparators(QString(text).remove(" ").remove(","));

    setText(reformatExpr);

    // reformat text.
    int oldLength = QString(text).length();
    int newLength = reformatExpr.length();

    int pos;
    if (newLength > oldLength)
        pos = oldPosition + (newLength - oldLength);
    else
//        pos = oldPosition - (oldLength - newLength);
        pos = oldPosition;
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
    QString tmp_2 = tmp.right(1);

    if(tmp_2 >= '0' && tmp_2<= '9')
        insert("×sqrt()");
    else
        insert("sqrt()");
}

void ScienceExprCalculator::handleFunction_10n()
{
    int pos = cursorPosition();
    QString expr = text();
    QString tmp = expr.left(pos);
    QString tmp_2 = tmp.right(1);

    if(tmp_2 >= '0' && tmp_2<= '9')
        insert("×10^");
    else
        insert("10^");
}

void ScienceExprCalculator::handleFunction_Opposite()
{

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

void ScienceExprCalculator::handleFunction_Backspace()
{
    int currentPos = cursorPosition();
    QString expr = text();
    if(currentPos !=0 && (expr.at(currentPos - 1) == " "))
    {
        setCursorPosition(currentPos - 1);

        qDebug() << "cursorPosition: ";
        qDebug() << cursorPosition();

        backspace();
    }
    else
        backspace();
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
    qDebug() << "m_FE:";
    qDebug() << m_FE;
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
