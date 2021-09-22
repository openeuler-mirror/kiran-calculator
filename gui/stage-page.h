#ifndef STAGEPAGE_H
#define STAGEPAGE_H
#include "quantity.h"
#include <QListWidget>

class Evaluator;
class ProgrammerExprCalculator;

class StagePage : public QListWidget
{
    Q_OBJECT
public:
    StagePage(QWidget *parent=nullptr);
    static int m_currentFormat;


public slots:
    void setStageResult();
    void setStageErrorMessage();
    void setStageNanMessage();
    void NumFormatStageResult();
    void handleStagePageItem(QListWidgetItem *);
    void stageFormatChanged(int );


    void receiveCalculatedExpr(const QString&);
    void receiveCalculatedQuantity(const Quantity&);
protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                         const QEvent *event) const override;
private:
    Evaluator* m_evaluator;
    ProgrammerExprCalculator* m_programmerExpr;
    QString m_saveExpr;
    Quantity m_saveQuantity;
//    QString m_saveError;

signals:
    void stageExprSelected(const QString &);

};

#endif // STAGEPAGE_H
