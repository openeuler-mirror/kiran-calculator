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

void StagePage::setHistoryResult(const QString & result)
{
    clear();
    clearSelection();
    QString stageResult = Utils::reformatSeparators(result);
    QListWidget::addItem(stageResult);
    item(0)->setTextAlignment(Qt::AlignRight);
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
        QListWidget::addItem(reformatExpr.replace("-",QString::fromUtf8("−")));
        item(0)->setTextAlignment(Qt::AlignRight);
    }
}

void StagePage::setStageResult()
{
    clear();
    clearSelection();
    QString stageList;
    QString stageExpr;
    stageExpr = Utils::reformatSeparators(m_saveExpr);

    if(!m_saveQuantity.isNan())
    {
        stageList.append(stageExpr + "=" + Utils::reformatSeparators(NumberFormatter::format(m_saveQuantity)));
    }
    QListWidget::addItem(stageList);
    item(0)->setTextAlignment(Qt::AlignRight);

//    qDebug() << "stagePage-font:";
//    qDebug() << item(0)->data(Qt::FontRole).value<QFont>();
//    qDebug() << item(0)->font();
}

void StagePage::setStageErrorMessage()
{
    clear();
    clearSelection();
    QString errorMessage = "表达式错误";
    QListWidget::addItem(errorMessage);
    item(0)->setTextAlignment(Qt::AlignRight);
}

void StagePage::setStageNanMessage()
{
    clear();
    clearSelection();
    QString nanMessage = "结果无定义";
    QListWidget::addItem(nanMessage);
    item(0)->setTextAlignment(Qt::AlignRight);
}

QItemSelectionModel::SelectionFlags StagePage::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if(event==nullptr){
        return QItemSelectionModel::NoUpdate;
    }
    if(event->type() == QEvent::MouseMove)
        return QItemSelectionModel::NoUpdate;
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


void StagePage::handleStagePageItem(QListWidgetItem *item)
{
    QString str = item->text();
    //以'='号进行分割
    str = str.section('=',-2,-2);
    qDebug() << "handleStagePageItem:" + str;
    emit stageExprSelected(str);
    clearSelection();
}

