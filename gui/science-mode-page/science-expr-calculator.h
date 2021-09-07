#ifndef SCIENCEEXPRCALCULATOR_H
#define SCIENCEEXPRCALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include "core/sessionhistory.h"
#include "core/session.h"

class Evaluator;
class Session;
class CNumber;
class QEvent;
class QKeyEvent;
class QTreeWidget;
class QWidget;
class HistoryEntry;
class Settings;

class ScienceExprCalculator : public QLineEdit
{
    Q_OBJECT
public:
    ScienceExprCalculator(QWidget* parent = nullptr);
    void setSession(Session*);
    void handleFunction_10n();
    void handleFunction_Sqrt();
    void handleFunction_Opposite();
    void handleFunction_Pi();
    void handleFunction_Backspace();

    static bool m_FE;

    enum AngleUnit{
        Angle_Unit_Degree,Angle_Unit_Radian,Angle_Unit_Gradian,
    };

public slots:
    void insert(const QString&);
    void setText(const QString&);
    void scienceExprCalc();
    void reformatShowExpr(const QString& text);
    void handleFunction_SwitchAngleUnit(int );
    void handleFunction_FE();


protected:
    void keyPressEvent(QKeyEvent *) override;
protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_scienceHistory;
    int m_currentScienceHistoryIndex;
    Session* m_scienceSession;

signals:
    void scienceExprCalcMessage(const QString&);
    void scienceExprCalcQuantity(const Quantity&);
    void scienceHistoryChanged();
    void scienceEqualPressed();

    void scienceToStageExprFormat(const QString&);
    void scienceToStageQuantity(const Quantity&);
    void scienceStageChanged();
};

#endif // SCIENCEEXPRCALCULATOR_H
