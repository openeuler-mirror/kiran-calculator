#include "mode-list.h"
#include <QEvent>
#include <QMouseEvent>
ModeList::ModeList(QWidget *parent)
    :QListWidget(parent)
{

}

QItemSelectionModel::SelectionFlags ModeList::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    //NOTE:为了避免一些环境下会出现ItemFocus直接设置选中的情况
    if(event==nullptr){
        return QItemSelectionModel::NoUpdate;
    }
    if(event->type() == QEvent::MouseMove)
        return QItemSelectionModel::NoUpdate;
    if((event != nullptr) && (event->type() == QEvent::MouseButtonPress))
    {
        const QMouseEvent* mouseEvent = (QMouseEvent*) event;           // 转换为鼠标事件
        if((mouseEvent->modifiers() & Qt::ControlModifier) != 0)
        {
            return QItemSelectionModel::NoUpdate;
        }
    }
    return QListWidget::selectionCommand(index,event);
}
