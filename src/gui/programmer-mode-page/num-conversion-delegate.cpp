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
#include "num-conversion-delegate.h"
#include "num-conversion.h"
#include <QPainter>
#include <QDebug>
#include <style-helper.h>

const int NAME_WIDTH = 64;
NumConversionDelegate::NumConversionDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void NumConversionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString expression = index.model()->data(index,Qt::DisplayRole).toString();;
    NumConversion *numList = qobject_cast<NumConversion *>(option.styleObject);

    // option.rect.x()和option.rect.x() 均为0
    QRect nameRect(option.rect.x() + 6,option.rect.y(), 64,24);
    QRect numRect(option.rect.x() + NAME_WIDTH + 4, option.rect.y(), 268, 24);

    QColor  fontColor, hoverColor, selectedColor;
    auto style = Kiran::Theme::StyleHelper::getDefault();
    if(style->paletteType() ==  Kiran::Theme::PALETTE_DARK)
    {
        fontColor = QColor("#FFFFFF");
        hoverColor = QColor(255,255,255);
    }
    else
    {
        fontColor = QColor("#222222");
        hoverColor = QColor(0,0,0);
    }
    hoverColor.setAlphaF(0.3);
    selectedColor = QColor("#2eb3ff");

    QFont font,textFont;
    font.setPixelSize(12);
    font.setFamily("Noto Sans CJK SC Regular");
    textFont.setPixelSize(14);
    textFont.setFamily("Noto Sans CJK SC Regular");
    if(index == numList->currentIndex())
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(selectedColor);
        painter->drawRect(QRect(option.rect.x(),option.rect.y(), 64,24));
    }

    if(option.state & QStyle::State_MouseOver)
    {
        painter->setBrush(hoverColor);
        painter->setPen(Qt::NoPen);      //边框
        painter->drawRect(QRect(option.rect.x() , option.rect.y(), 332, 24));
    }

    //结果字体绘制
    painter->setFont(font);
    painter->setPen(fontColor);
    if (index.row() == 3) {
        if (expression.size() > 39) {
            //显示64位时
            //第一行
            painter->drawText(QRect (option.rect.x() + NAME_WIDTH + 4, option.rect.y(), 268, 12), expression.mid(0, 39), Qt::AlignLeft | Qt::AlignVCenter);
            //第二行
            painter->drawText(QRect (option.rect.x() + NAME_WIDTH + 4, option.rect.y() + 12, 268, 12), expression.mid(40, -1), Qt::AlignLeft | Qt::AlignVCenter);
        } else {
            painter->drawText(numRect, expression, Qt::AlignLeft | Qt::AlignVCenter);
        }
    } else
        painter->drawText(numRect, expression, Qt::AlignLeft | Qt::AlignVCenter);

    painter->setFont(textFont);
    switch (index.row()) {
    case 0:
       painter->drawText(nameRect, "HEX", Qt::AlignLeft | Qt::AlignVCenter);
       break;
    case 1:
       painter->drawText(nameRect, "DEC", Qt::AlignLeft | Qt::AlignVCenter);
       break;
    case 2:
       painter->drawText(nameRect, "OCT", Qt::AlignLeft | Qt::AlignVCenter);
       break;
    case 3:
       painter->drawText(nameRect, "BIN", Qt::AlignLeft | Qt::AlignVCenter);
       break;
    }

}

QSize NumConversionDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        return QSize(332, 24);
}

void NumConversionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
