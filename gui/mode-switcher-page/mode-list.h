#ifndef MODELIST_H
#define MODELIST_H

#include <QListWidget>

class ModeList : public QListWidget
{
    Q_OBJECT
public:
    ModeList(QWidget *parent);

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                         const QEvent *event) const override;

};

#endif // MODELIST_H
