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

#include "history-recoder.h"
#include "core/sessionhistory.h"
#include "core/evaluator.h"
#include "core/session.h"
#include "core/numberformatter.h"
#include "programmer-mode-page/programmer-expr-calculator.h"
#include "utils.h"

#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QEvent>
#include <QDesktopWidget>
#include <style-palette.h>

HistoryRecoder::HistoryRecoder(QWidget *parent) : QListWidget(parent)
{
    setWordWrap(true);
    adjustSize();

    connect(this, SIGNAL(itemClicked(QListWidgetItem *)),this,
            SLOT(handleItem(QListWidgetItem *)));
    connect(Kiran::StylePalette::instance(),&Kiran::StylePalette::themeChanged,this,&HistoryRecoder::updateHistory);
}

void HistoryRecoder::setSession(Session *session)
{
    m_session = session;
}

void HistoryRecoder::setCalculateMode(int mode)
{
    m_currentMode = mode;
}

//重新更新历史记录时，要释放之前的内存
//TODO:优化历史代码
void HistoryRecoder::updateHistory()
{
    if(m_historyWidgetList.count() != 0)
    {
        //All items will be permanently deleted
        this->clear();
        for(int i = 0; i < m_historyWidgetList.count() ; ++i)
        {
            delete m_historyWidgetList.value(i);
        }
        m_labelExprList.clear();
        m_labelValueList.clear();
        m_vboxlayoutList.clear();
        m_historyWidgetList.clear();
        m_historyItemList.clear();
    }

    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
    clear();
    clearSelection();
    QStringList historyList;
    QString expression;
    Quantity value;

    for(int i = 0; i<history.count();i++)
    {
        auto labelExpr = new QLabel();
        auto labelValue = new QLabel();
        auto vboxlayout = new QVBoxLayout();
        auto historyWidget = new QWidget();
        auto historyItem = new QListWidgetItem();

        m_labelExprList << labelExpr;
        m_labelValueList << labelValue;
        m_vboxlayoutList << vboxlayout;
        m_historyWidgetList << historyWidget;
        m_historyItemList << historyItem;

        expression = history[i].expr();
        value = history[i].result();
        QString labelExprObjectName = "labelExpr_";
        labelExprObjectName.append(QString::number(i));
        labelExpr->setObjectName(labelExprObjectName);

        QString labelValueObjectName = "labelValue_";
        labelValueObjectName.append(QString::number(i));
        labelValue->setObjectName(labelValueObjectName);

        QString reformatExpr;
        if(m_currentMode == Calculation_Mode_Programmer)
        {
            ProgrammerExprCalculator programmerExprCalculator;
            QString expressionConverted = programmerExprCalculator.scanAndExec(Num_Format_Dec,m_currentFormat,expression);

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
        }
        labelExpr->setWordWrap(true);
        if(m_currentMode == Calculation_Mode_Programmer)
            labelExpr->setText(historyWordWrap((reformatExpr + "="), 24));
        else
            labelExpr->setText(historyWordWrap((Utils::reformatSeparators(expression) + "="), 24));

        labelExpr->setStyleSheet("padding-right:4px;color:#919191;font-size:14px;font-family: Noto Sans CJK SC Regular;");
        labelExpr->setAlignment(Qt::AlignRight);
        labelExpr->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        labelExpr->setFixedWidth(214);
        labelExpr->setMinimumHeight(18);
        labelExpr->adjustSize();
        vboxlayout->addWidget(labelExpr);

        if(!value.isNan())
        {
            labelValue->setWordWrap(true);
            if(m_currentMode == Calculation_Mode_Programmer)
            {
                QString reformatValue;
                switch (m_currentFormat) {
                case Num_Format_Hex:
                    reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x"), 16);
                    break;
                case Num_Format_Dec:
                    reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed()), 10);
                    break;
                case Num_Format_Oct:
                    reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o"), 8);
                    break;
                case Num_Format_Bin:
                    reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b"), 2);
                    break;
                }
                labelValue->setText(historyWordWrap(reformatValue.replace("-",QString::fromUtf8("−")), 16));
            }
            else if(m_currentMode == Calculation_Mode_Science)
            {
                if(m_historyFEIndex.contains(i))
                {
                    labelValue->setText(historyWordWrap(Utils::reformatSeparators(DMath::format(value, Quantity::Format::Scientific())), 16));
                }
                else
                {
                    QString scienceValueLength = NumberFormatter::format(value);
                    if(scienceValueLength.remove(".").length() > 24)
                        labelValue->setText(historyWordWrap(Utils::reformatSeparators(DMath::format(value, Quantity::Format::Scientific())), 16));
                    else
                        labelValue->setText(historyWordWrap(Utils::reformatSeparators(NumberFormatter::format(value)), 16));
                }
            }
            else
            {
                QString standardValueLength =  NumberFormatter::format(value);
                if(standardValueLength.remove(".").length() > 16)
                    labelValue->setText(historyWordWrap(Utils::reformatSeparators(DMath::format(value, Quantity::Format::Scientific())), 16));
                else
                    labelValue->setText(historyWordWrap(Utils::reformatSeparators(NumberFormatter::format(value)), 16));

            }

            if(Kiran::StylePalette::instance()->paletteType() == Kiran::PALETTE_DARK)
                labelValue->setStyleSheet("color:#FFFFFF;font-size:24px;font-family: Noto Sans CJK SC Regular;padding-bottom:14px;");
            else
                labelValue->setStyleSheet("color:#222222;font-size:24px;font-family: Noto Sans CJK SC Regular;padding-bottom:14px;");
            labelValue->setAlignment(Qt::AlignRight);
            labelValue->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            labelValue->setFixedWidth(214);
            labelValue->setMinimumHeight(30);
            labelValue->adjustSize();
            vboxlayout->addWidget(labelValue);

        }

        vboxlayout->setMargin(0);
        vboxlayout->setSpacing(4);

        historyWidget->setLayout(vboxlayout);

        historyWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        historyWidget->setFixedWidth(214);
        historyWidget->setMinimumHeight(54);

        historyWidget->resize(204,labelExpr->rect().height() + labelValue->rect().height()
                                         + vboxlayout->spacing());

        historyItem->setSizeHint(QSize(214,
                                         labelExpr->rect().height() + labelValue->rect().height()
                                             + vboxlayout->spacing()));

        this->addItem(historyItem);
        this->setItemWidget(historyItem,historyWidget);

    }
    this->scrollToBottom();

}

void HistoryRecoder::historyFormatChanged(int format)
{
    //不需要记录转换前的进制，历史记录每次都是从十进制进行转换
    m_currentFormat = format;
}

void HistoryRecoder::historyFEChanged()
{
    if(!m_FE)
        m_FE = true;
    else
        m_FE = false;
}

void HistoryRecoder::historyFEIndex(QList<int> index)
{
    m_historyFEIndex = index;
}


void HistoryRecoder::clearHistory()
{
    m_session->clearHistory();
    m_historyFEIndex.clear();
    emit historyClearSuccess();
    updateHistory();
}

bool HistoryRecoder::isHistoryEmpty()
{
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
        expr.insert(limitLength*i + i-1,"\n");
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
    if((event != nullptr) && (event->type() == QEvent::MouseButtonPress))
    {
        const QMouseEvent* mouseEvent = (QMouseEvent*) event;
        if((mouseEvent->modifiers() & Qt::ControlModifier) != 0)
        {
            return QItemSelectionModel::NoUpdate;
        }
        if(mouseEvent->button() == Qt::RightButton)
        {
            return QItemSelectionModel::NoUpdate;
        }
    }
    return QListWidget::selectionCommand(index,event);
}

void HistoryRecoder::handleItem(QListWidgetItem *item)
{

    auto widget = itemWidget(item);
    QString labelValueName = "labelValue_";
    labelValueName.append(QString::number(row(item)));
    QString labelExprName = "labelExpr_";
    labelExprName.append(QString::number(row(item)));

    auto labelValue = widget->findChild<QLabel *>(labelValueName);
    auto labelExpr = widget->findChild<QLabel *>(labelExprName);

    QString result = labelExpr->text() + labelValue->text();

    emit valueSelected(labelValue->text());
    emit resultSelected(result.remove(" ").remove("\n"));
    clearSelection();
}

