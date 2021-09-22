#ifndef SCIENCEEXPRCALCULATOR_H
#define SCIENCEEXPRCALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include "core/sessionhistory.h"
#include "core/session.h"
#include "sselection.h"
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
    void handleFunction_Square();
    void handleFunction_Xn();
    void handleFunction_Sin();
    void handleFunction_Cos();
    void handleFunction_Tan();
    void handleFunction_Log();
    void handleFunction_Opposite();
    bool expressionInFunc(QString &);
    void handleFunction_Pi();
    void handleFunction_Backspace();
    void handleFunction_Exp();         //科学计数法，不是指数函数
    void handleFunction_Reciprocal();
    void handleFunction_Factorials();
    bool judgeInsertPos();     //判断光标位置，禁止在函数名中间插入

    SSelection getSelection() { return m_selected; }    //选中

    void initMenuAndAction();

    static bool m_FE;
    static bool m_isShift;
    static bool m_isHYP;


public slots:
    void insert(const QString&);
    void handleInsertText(const QString&);
    void setText(const QString&);
    void scienceExprCalc();
    void reformatShowExpr(const QString& text);
    void handleFunction_SwitchAngleUnit(int );
    void handleFunction_FE();
    void handleFunction_Shift();
    void handleFunction_HYP();
    void disableSelectText();

    void showMenu(const QPoint &point);
    void copyResultToClipboard();
    void paste();
    void exprSelectAll();     //selectAll重名了

protected:
    void keyPressEvent(QKeyEvent *) override;

protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_scienceHistory;
    int m_currentScienceHistoryIndex;
    Session* m_scienceSession;
    QList<QString> m_funclist;

    SSelection m_selected;

    //输入栏右键菜单
    QMenu *m_menu;
    QAction *m_copy;
    QAction *m_paste;
    QAction *m_selectAll;
signals:
    void scienceExprCalcMessage(const QString&);
    void scienceExprCalcQuantity(const Quantity&);
    void scienceExprCalcError();
    void scienceExprCalcNan();

    void scienceHistoryChanged();
    void scienceEqualPressed();

    void scienceToStageExprFormat(const QString&);
    void scienceToStageQuantity(const Quantity&);
    void scienceStageChanged();

    void scienceFEChanged();
};

#endif // SCIENCEEXPRCALCULATOR_H
