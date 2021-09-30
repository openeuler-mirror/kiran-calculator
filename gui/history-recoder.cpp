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


    setAlternatingRowColors(true); // 行与行之间交替颜色不同,暂不启用
    setTextElideMode(Qt::ElideNone);

    m_exprFont.setPixelSize(14);
    m_resultFont.setPixelSize(24);
    m_exprFont.setFamily("Noto Sans CJK SC Light");
    m_resultFont.setFamily("Noto Sans CJK SC Light");

    setWordWrap(true);
//    setResizeMode(QListView::Adjust);
    adjustSize();
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
//        historyList.append(Utils::reformatSeparatorsPro(expression, 10) + "=");

        historyList.append(
                    historyWordWrap((Utils::reformatSeparatorsPro(expression, 10) + "="), 26)
                    );
        if(!value.isNan())
        {
            historyList.append(
                        historyWordWrap(Utils::reformatSeparatorsPro(NumberFormatter::format(value), 10), 17)
                        );
        }
    }
    QListWidget::addItems(historyList);

    for(int i=0; i<historyList.count();i++)
    {
        item(i)->setTextAlignment(Qt::AlignRight);
        if(i%2)
        {

            item(i)->setFont(m_resultFont);
            item(i)->setTextColor(QColor("#FFFFFF"));
        }
        else
        {
            item(i)->setFont(m_exprFont);
            item(i)->setTextColor(QColor("#919191"));
        }
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
                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
                break;
            case Num_Format_Dec:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed()), 10);
                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
                break;
            case Num_Format_Oct:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o"), 8);
                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
                break;
            case Num_Format_Bin:
                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b"), 2);
                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
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
    emit historyClearSuccess();
    updateScienceHistory();
}

void HistoryRecoder::clearStandardHistory()
{
    m_session->clearHistory();
    emit historyClearSuccess();
    updateStandardHistory();
}

void HistoryRecoder::clearProgrammerHistory()
{
    m_session->clearHistory();
    emit historyClearSuccess();
    updateProgrammerHistory();
}

bool HistoryRecoder::isHistoryEmpty()
{
    qDebug() << "current count:";
    qDebug() << count();
    if(count())
        return true;
    else
        return false;
}

QString HistoryRecoder::historyWordWrap(const QString &text,int limitLength)
{
    QString expr = text;
    int length = text.count();
    int rowCount = 1;

    if(length <= limitLength)
        return expr;

    if(length % limitLength)
        rowCount = (length/limitLength) + 1;
    else
        rowCount = length/limitLength;

    for(int i = 1; i<rowCount; i++)
    {
        expr.insert(limitLength*i,"\n");
    }
    return expr;
}

QItemSelectionModel::SelectionFlags HistoryRecoder::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if(event==nullptr)
        return QItemSelectionModel::NoUpdate;
    if(event->type() == QEvent::MouseMove)
    {
        return QItemSelectionModel::NoUpdate;
    }
    if(event->type() == QEvent::MouseButtonRelease)
    {

    }
    if((event != nullptr) && (event->type() == QEvent::MouseButtonPress))
    {
        const QMouseEvent* mouseEvent = (QMouseEvent*) event;
        if((mouseEvent->modifiers() & Qt::ControlModifier) != 0)
        {
            return QItemSelectionModel::NoUpdate;
        }
    }
    return QListWidget::selectionCommand(index,event);
}

void HistoryRecoder::handleItem(QListWidgetItem *item)
{
    qDebug() << "handleItem:"+item->text();
    emit exprSelected(item->text());
    clearSelection();
}


