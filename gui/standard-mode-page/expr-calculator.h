#ifndef ExprCalculator_H
#define ExprCalculator_H

#include <QWidget>
#include "core/sessionhistory.h"
#include "core/session.h"
#include <QLineEdit>

class Evaluator;
class Session;
class CNumber;
class QEvent;
class QKeyEvent;
class QWidget;
class HistoryEntry;
class Settings;
class Session;
class ExprCalculator : public QLineEdit
{
    Q_OBJECT
public:
    ExprCalculator(QWidget *parent = nullptr);

    void handleFunction_Sqrt();
    void handleFunction_Opposite();
    void setSession(Session*);
    bool expressionInFunc(QString &);

protected:
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void evaluate();
    void insert(const QString&);
    void setText(const QString&);
    void exprCalc();
    void reformatShowExpr(const QString&);


protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;

    QList<HistoryEntry> m_standardHistory;
    int m_currentStandardHistoryIndex;
    Session* m_standardSession;
    QString m_savedCurrentEditor;
    QList<QString> m_funclist;


signals:
    void exprCalcMessageDec(const QString&);
    void exprCalcQuantityDec(const Quantity&);

    void equalPressed();
    void standardHistoryChanged( );

    void standardToStageExprFormat(const QString&);
    void standardToStageQuantity(const Quantity&);
    void standardStageChanged();

};

#endif // ExprCalculator_H
