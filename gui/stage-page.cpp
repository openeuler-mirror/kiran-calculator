#include "stage-page.h"

#include "core/evaluator.h"

#include "core/numberformatter.h"
#include "programmer-mode-page/programmer-expr-calculator.h"
#include "utils.h"
#include <QDebug>


int StagePage::m_currentFormat = 1;

#define FORMAT_DEC 1

StagePage::StagePage(QWidget *parent) : QListWidget(parent)
{
    connect(this,SIGNAL(itemClicked(QListWidgetItem *)),this,
            SLOT(handleStagePageItem(QListWidgetItem *)));
}

void StagePage::stageFormatChanged(int format)
{
    m_currentFormat = format;
}

void StagePage::receiveCalculatedExpr(const QString &expr)
{
    m_saveExpr = expr;
}

void StagePage::receiveCalculatedQuantity(const Quantity &quantity)
{
    m_saveQuantity = quantity;
}

void StagePage::NumFormatStageResult()
{
    m_programmerExpr = new ProgrammerExprCalculator();
    clear();
    clearSelection();
    QString stageExprConverted;
    stageExprConverted = m_programmerExpr->scanAndExec(FORMAT_DEC,m_currentFormat,m_saveExpr);

    QString expr;
    QString reformatExpr;
    if(!stageExprConverted.isEmpty())
    {
        switch (m_currentFormat) {
        case 0:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 16);
            break;
        case 2:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 8);
            break;
        case 3:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 2);
            break;
        default:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(","), 10);
            break;
        }
        qDebug() << "reformatExpr:" + reformatExpr;
        QListWidget::addItem(reformatExpr);
        item(0)->setTextAlignment(Qt::AlignRight);
    }
}

void StagePage::setStageResult()
{
    clear();
    clearSelection();
    QString stageList;
    QString stageExpr;
    stageExpr = Utils::reformatSeparatorsPro(m_saveExpr, 10);

    if(!m_saveQuantity.isNan())
    {
        stageList.append(stageExpr + "=" + Utils::reformatSeparatorsPro(NumberFormatter::format(m_saveQuantity), 10));
    }
    QListWidget::addItem(stageList);
    item(0)->setTextAlignment(Qt::AlignRight);
}


void StagePage::handleStagePageItem(QListWidgetItem *item)
{
    QString str = item->text();
    //以'='号进行分割
    str = str.section('=',-2,-2);
    qDebug() << "handleStagePageItem:" + str;
    emit stageExprSelected(str);
    clearSelection();
}

