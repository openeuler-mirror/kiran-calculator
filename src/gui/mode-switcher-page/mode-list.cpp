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

#include "mode-list.h"
#include <QEvent>
#include <QMouseEvent>
ModeList::ModeList(QWidget *parent)
    : QListWidget(parent)
{
}

QItemSelectionModel::SelectionFlags ModeList::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event == nullptr)
    {
        return QItemSelectionModel::NoUpdate;
    }
    if (event->type() == QEvent::MouseMove)
        return QItemSelectionModel::NoUpdate;
    if ((event != nullptr) && (event->type() == QEvent::MouseButtonPress))
    {
        const QMouseEvent *mouseEvent = (QMouseEvent *)event;  // 转换为鼠标事件
        if ((mouseEvent->modifiers() & Qt::ControlModifier) != 0)
        {
            return QItemSelectionModel::NoUpdate;
        }
        if (mouseEvent->button() == Qt::RightButton)
            return QItemSelectionModel::NoUpdate;
    }
    return QListWidget::selectionCommand(index, event);
}
