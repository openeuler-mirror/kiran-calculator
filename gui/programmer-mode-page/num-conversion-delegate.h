#ifndef NUMCONVERSIONDELEGATE_H
#define NUMCONVERSIONDELEGATE_H

#include <QStyledItemDelegate>

class NumConversionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    NumConversionDelegate(QObject *parent = 0);

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const override;

private:
    QModelIndex m_focusindex;

};

#endif // NUMCONVERSIONDELEGATE_H
