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
#include "num-conversion.h"
#include "core/sessionhistory.h"
#include "core/session.h"
#include "core/numberformatter.h"
#include "core/evaluator.h"
#include "utils.h"
#include <QDebug>
#include <QString>

NumConversion::NumConversion(QWidget* parent) : QTableWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QTableWidgetItem*)),this,
            SLOT(activateNumConversion(QTableWidgetItem*)));

    m_evaluator = Evaluator::instance();
    setTextElideMode(Qt::ElideNone);
//    adjustSize();

//    setCurrentItem(item(1,0));

}

void NumConversion::showNumFormatConverted(const Quantity & quantity)
{
    //重载运算符'+'  使得进制和模式可以组合
    QString formatBin = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
    QString formatOct = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
    QString formatHex = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
    QString formatDec = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
    //    QString formatDec = NumberFormatter::format(quantity);

    qDebug() << "NumConversion_formatDec:" + formatDec;

    //二进制补0
    int i = formatBin.length() % 4;
    if (formatBin != "0") {
        while (i != 4 && i != 0) {
            formatBin.insert(0, "0");
            i++;
        }
    }

    formatHex = Utils::reformatSeparatorsPro(formatHex, 16);
    formatOct = Utils::reformatSeparatorsPro(formatOct, 8);
    formatBin = Utils::reformatSeparatorsPro(formatBin, 2);
    formatDec = Utils::reformatSeparatorsPro(formatDec, 10);

    QFont font;
    font.setPixelSize(10);
    setFont(font);


    if(formatBin.length() > 50 )
    {
        formatBin.insert(50,"\n");
    }

    qDebug() << "formatBin:";
    qDebug() << formatBin;
    item(0,0)->setText(formatHex);
    item(1,0)->setText(formatDec);
    item(2,0)->setText(formatOct);
    item(3,0)->setText(formatBin);

    qDebug() << item(3,0)->sizeHint();

    resizeRowToContents(3);
//    adjustSize();


}

void NumConversion::activateNumConversion(QTableWidgetItem* item)
{
    qDebug() << "row:";
    qDebug() << item->row();

    emit numConvered(item->row());
    emit refreshNumFormatHistory( );
    emit refreshNumFormatStage();
}

//重新定义选择行为，禁止拖动和反选
QItemSelectionModel::SelectionFlags NumConversion::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    //NOTE:为了避免一些环境下会出现ItemFocus直接设置选中的情况
    // 默认选中10进制
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
    return QTableWidget::selectionCommand(index,event);
}

void NumConversion::clearItems()
{
    qDebug() << "clearItems";
    item(0,0)->setText("");
    item(1,0)->setText("");
    item(2,0)->setText("");
    item(3,0)->setText("");
}
