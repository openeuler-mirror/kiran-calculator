#include "history-recoder.h"
#include "core/sessionhistory.h"
#include "core/evaluator.h"
#include "core/session.h"
#include "core/numberformatter.h"
#include "programmer-mode-page/programmer-expr-calculator.h"
#include "utils.h"
#include "general-enum.h"
#include <QDebug>
#include <QListWidget>

#define FORMAT_DEC 1
int HistoryRecoder::m_currentFormat = 1;
bool HistoryRecoder::m_FE = false; //科学计算法模式默认关闭

HistoryRecoder::HistoryRecoder(QWidget *parent) : QListWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)),this,
            SLOT(handleItem(QListWidgetItem *)));
}

void HistoryRecoder::setSession(Session *session)
{
    m_session = session;
}

void HistoryRecoder::updateStandardHistory()
{
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
    clear();
    clearSelection();
    QStringList historyList;
    QString expression;
    Quantity value;

    for(int i=0; i<history.count();i++)
    {
        expression = history[i].expr();
        value = history[i].result();
        historyList.append(Utils::reformatSeparatorsPro(expression, 10) + "=");
        if(!value.isNan())
        {
            historyList.append(Utils::reformatSeparatorsPro(NumberFormatter::format(value), 10));
        }
    }
    QListWidget::addItems(historyList);

    for(int i=0; i<historyList.count();i++)
    {
        item(i)->setTextAlignment(Qt::AlignRight);
    }
    QListWidget::scrollToBottom();

}

void HistoryRecoder::updateScienceHistory()
{
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
    clear();
    clearSelection();
    QStringList historyList;
    QString expression;
    Quantity value;

    for(int i=0; i<history.count();i++)
    {
        expression = history[i].expr();
        value = history[i].result();
        historyList.append(Utils::reformatSeparatorsPro(expression, 10) + "=");
        if(!value.isNan())
        {
            if(m_FE)
                historyList.append(Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Scientific()), 10));
            else
                historyList.append(Utils::reformatSeparatorsPro(NumberFormatter::format(value), 10));
        }
    }
    QListWidget::addItems(historyList);

    for(int i=0; i<historyList.count();i++)
    {
        item(i)->setTextAlignment(Qt::AlignRight);
    }
    QListWidget::scrollToBottom();
}

void HistoryRecoder::historyFormatChanged(int format)
{
    //不需要记录转换前的进制，历史记录每次都是从十进制进行转换
    m_currentFormat = format;
}


//更新程序员模式的历史记录
void HistoryRecoder::updateProgrammerHistory()
{
    m_programmerExpr = new ProgrammerExprCalculator();
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
    clear();
    clearSelection();
    QStringList historyList;
    QString expression;
    Quantity value;

    for(int i=0; i<history.count();i++)
    {
        expression = history[i].expr();
        QString expressionConverted = m_programmerExpr->scanAndExec(FORMAT_DEC,m_currentFormat,expression);
        QString reformatExpr;
        QString reformatValue;
        switch (m_currentFormat) {
        case Num_Format_Hex:
            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 16);
            break;
        case Num_Format_Dec:
            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(","), 10);
            break;
        case Num_Format_Oct:
            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 8);
            break;
        case Num_Format_Bin:
            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 2);
            break;
        }
        historyList.append(reformatExpr + "=");
        value = history[i].result();
        if(!value.isNan())
        {
            switch (m_currentFormat) {
            case Num_Format_Hex:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x"), 16);
                historyList.append(reformatValue);
                break;
            case Num_Format_Dec:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed()), 10);
                historyList.append(reformatValue);
                break;
            case Num_Format_Oct:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o"), 8);
                historyList.append(reformatValue);
                break;
            case Num_Format_Bin:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b"), 2);
                historyList.append(reformatValue);
                break;
            }
        }
    }

    QListWidget::addItems(historyList);
    for(int i=0; i<historyList.count();i++)
    {
        item(i)->setTextAlignment(Qt::AlignRight);
    }
    QListWidget::scrollToBottom();
}

void HistoryRecoder::historyFEChanged()
{
    if(!m_FE)
        m_FE = true;
    else
        m_FE = false;
}

void HistoryRecoder::clearScienceHistory()
{
    m_session->clearHistory();
    updateScienceHistory();
}

void HistoryRecoder::clearStandardHistory()
{
    m_session->clearHistory();
    updateStandardHistory();
}

void HistoryRecoder::clearProgrammerHistory()
{
    m_session->clearHistory();
    updateProgrammerHistory();
}

void HistoryRecoder::handleItem(QListWidgetItem *item)
{
    qDebug() << "handleItem:"+item->text();
    emit exprSelected(item->text());
    clearSelection();
}


