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
#include "general-enum.h"
#include <QDebug>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>

#define FORMAT_DEC 1
int HistoryRecoder::m_currentFormat = Num_Format_Dec;
int HistoryRecoder::m_currentMode = Calculation_Mode_Standard;
bool HistoryRecoder::m_FE = false; //科学计算法模式默认关闭

HistoryRecoder::HistoryRecoder(QWidget *parent) : QListWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)),this,
            SLOT(handleItem(QListWidgetItem *)));

//    setAlternatingRowColors(true); // 行与行之间交替颜色不同,暂不启用
//    setTextElideMode(Qt::ElideNone);

//    m_exprFont.setPixelSize(14);
//    m_resultFont.setPixelSize(24);
//    m_exprFont.setFamily("Noto Sans CJK SC Light");
//    m_resultFont.setFamily("Noto Sans CJK SC Light");

    setWordWrap(true);
    adjustSize();
}

void HistoryRecoder::setSession(Session *session)
{
    m_session = session;
}

void HistoryRecoder::setCalculateMode(int mode)
{
    m_currentMode = mode;
    qDebug() << "m_currentMode:";
    qDebug() << m_currentMode;
}

void HistoryRecoder::updateHistory()
{
    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
    clear();
    clearSelection();
    QStringList historyList;
    QString expression;
    Quantity value;

    QLabel *labelExpr[history.count()];    //指针数组
    QLabel *labelValue[history.count()];
    QVBoxLayout *vboxlayout[history.count()];
    QWidget *historyWidget[history.count()];
    QListWidgetItem *historyItem[history.count()];

    for(int i=0; i<history.count();i++)
    {
        expression = history[i].expr();
        value = history[i].result();
        labelExpr[i] = new QLabel();
        QString labelExprObjectName = "labelExpr_";
        labelExprObjectName.append(QString::number(i));
        labelExpr[i]->setObjectName(labelExprObjectName);

        labelValue[i] = new QLabel();
        QString labelValueObjectName = "labelValue_";
        labelValueObjectName.append(QString::number(i));
        labelValue[i]->setObjectName(labelValueObjectName);

        vboxlayout[i] = new QVBoxLayout();
        historyWidget[i] = new QWidget();
        historyItem[i] = new QListWidgetItem();


        if(m_currentMode == Calculation_Mode_Programmer)
        {
            m_programmerExpr = new ProgrammerExprCalculator();
            QString expressionConverted = m_programmerExpr->scanAndExec(FORMAT_DEC,m_currentFormat,expression);
            QString reformatExpr;

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


        labelExpr[i]->setWordWrap(true);
        if(m_currentMode == Calculation_Mode_Programmer)
            labelExpr[i]->setText(historyWordWrap((expression + "="), 24));
        else
            labelExpr[i]->setText(historyWordWrap((Utils::reformatSeparators(expression) + "="), 24));

        labelExpr[i]->setStyleSheet("padding-right:4px;color:#919191;font-size:14px;font-family: Noto Sans CJK SC Regular;");
        labelExpr[i]->setAlignment(Qt::AlignRight);
        labelExpr[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        labelExpr[i]->setFixedWidth(214);
        labelExpr[i]->setMinimumHeight(18);
        labelExpr[i]->adjustSize();
        vboxlayout[i]->addWidget(labelExpr[i]);

        if(!value.isNan())
        {
            labelValue[i]->setWordWrap(true);
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
                labelValue[i]->setText(historyWordWrap(reformatValue.replace("-",QString::fromUtf8("−")), 16));
            }
            else if(m_currentMode == Calculation_Mode_Science)
            {
                if(m_FE)
                    labelValue[i]->setText(historyWordWrap(Utils::reformatSeparators(DMath::format(value, Quantity::Format::Scientific())), 16));
                else
                    labelValue[i]->setText(historyWordWrap(Utils::reformatSeparators(NumberFormatter::format(value)), 16));
            }
            else
                labelValue[i]->setText(historyWordWrap(Utils::reformatSeparators(NumberFormatter::format(value)), 16));

            labelValue[i]->setStyleSheet("color:#FFFFFF;font-size:24px;font-family: Noto Sans CJK SC Regular;padding-bottom:14px;");
            labelValue[i]->setAlignment(Qt::AlignRight);
            labelValue[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            labelValue[i]->setFixedWidth(214);
            labelValue[i]->setMinimumHeight(30);
            labelValue[i]->adjustSize();
            vboxlayout[i]->addWidget(labelValue[i]);

            qDebug() << "history labelValue font:";
            qInfo() << labelValue[i]->font();

        }
        vboxlayout[i]->setMargin(0);
        vboxlayout[i]->setSpacing(4);

        historyWidget[i]->setLayout(vboxlayout[i]);

        historyWidget[i]->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        historyWidget[i]->setFixedWidth(214);
        historyWidget[i]->setMinimumHeight(54);

        historyWidget[i]->resize(204,labelExpr[i]->rect().height() + labelValue[i]->rect().height()
                                 + vboxlayout[i]->spacing());

        historyItem[i]->setSizeHint(QSize(214,
                                          labelExpr[i]->rect().height() + labelValue[i]->rect().height()
                                          + vboxlayout[i]->spacing()));

        QListWidget::addItem(historyItem[i]);
        QListWidget::setItemWidget(historyItem[i],historyWidget[i]);
    }

    QListWidget::scrollToBottom();
}

//void HistoryRecoder::updateScienceHistory()
//{
//    m_evaluator = Evaluator::instance();
//    m_evaluator->setSession(m_session);
//    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
//    clear();
//    clearSelection();
//    QStringList historyList;
//    QString expression;
//    Quantity value;

//    for(int i=0; i<history.count();i++)
//    {
//        expression = history[i].expr();
//        value = history[i].result();
//        historyList.append(Utils::reformatSeparatorsPro(expression, 10) + "=");
//        if(!value.isNan())
//        {
//            if(m_FE)
//                historyList.append(Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Scientific()), 10));
//            else
//                historyList.append(Utils::reformatSeparatorsPro(NumberFormatter::format(value), 10));
//        }
//    }
//    QListWidget::addItems(historyList);

//    for(int i=0; i<historyList.count();i++)
//    {
//        item(i)->setTextAlignment(Qt::AlignRight);
//    }
//    QListWidget::scrollToBottom();
//}

//更新程序员模式的历史记录
//void HistoryRecoder::updateProgrammerHistory()
//{
//    m_programmerExpr = new ProgrammerExprCalculator();
//    m_evaluator = Evaluator::instance();
//    m_evaluator->setSession(m_session);
//    QList<HistoryEntry> history = Evaluator::instance()->session()->historyToList();
//    clear();
//    clearSelection();
//    QStringList historyList;
//    QString expression;
//    Quantity value;

//    for(int i=0; i<history.count();i++)
//    {
//        expression = history[i].expr();
//        QString expressionConverted = m_programmerExpr->scanAndExec(FORMAT_DEC,m_currentFormat,expression);
//        QString reformatExpr;
//        QString reformatValue;
//        switch (m_currentFormat) {
//        case Num_Format_Hex:
//            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 16);
//            break;
//        case Num_Format_Dec:
//            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(","), 10);
//            break;
//        case Num_Format_Oct:
//            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 8);
//            break;
//        case Num_Format_Bin:
//            reformatExpr = Utils::reformatSeparatorsPro(expressionConverted.remove(" "), 2);
//            break;
//        }
//        historyList.append(reformatExpr + "=");
//        value = history[i].result();
//        if(!value.isNan())
//        {
//            switch (m_currentFormat) {
//            case Num_Format_Hex:
//                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x"), 16);
//                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
//                break;
//            case Num_Format_Dec:
//                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed()), 10);
//                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
//                break;
//            case Num_Format_Oct:
//                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o"), 8);
//                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
//                break;
//            case Num_Format_Bin:
//                reformatValue = Utils::reformatSeparatorsPro(DMath::format(value, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b"), 2);
//                historyList.append(reformatValue.replace("-",QString::fromUtf8("−")));
//                break;
//            }
//        }
//    }

//    QListWidget::addItems(historyList);
//    for(int i=0; i<historyList.count();i++)
//    {
//        item(i)->setTextAlignment(Qt::AlignRight);
//    }
//    QListWidget::scrollToBottom();
//}



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


void HistoryRecoder::clearHistory()
{
    m_session->clearHistory();
    emit historyClearSuccess();
    updateHistory();
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
    qDebug() << "handleItem_row:";
    qDebug() << QListWidget::row(item);

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


