#ifndef PROGRAMMEREXPRCALCULATOR_H
#define PROGRAMMEREXPRCALCULATOR_H
#include <QLineEdit>
#include <QWidget>
#include "core/session.h"
#include "core/sessionhistory.h"
#include "sselection.h"
class Evaluator;
class Session;
class CNumber;
class HistoryEntry;
class Settings;

class ProgrammerExprCalculator : public QLineEdit
{
    Q_OBJECT
public:
    ProgrammerExprCalculator(QWidget* parent = nullptr);

    void setSession(Session*);

    QString  AddPrefixForExpr(int ,const QString&);

    QString handleProgrammerFunction_Opposite_Dec(const QString&);
    void handleProgrammerFunction_Opposite();
    void handleProgrammerFunction_Backspace();

    bool curposInNumber(int );

    bool judgeInsertPos();     //判断光标位置，禁止在函数名中间插入
    void initMenuAndAction();

    static bool isNumber(QChar a);//判断是否为数字(分隔符)
    static int  m_currentFormat;
    static int  m_previousFormat;
    static bool m_numOutRange;

    SSelection getSelection() { return m_selected; }

public slots:
    void insert(const QString&);
    void handleProgrammerInsertText(const QString&);
    bool isNumberOutOfRange(const QString&);
    void setText(const QString&);
    void programmerExprCalc();
    void autoProgrammerExprCalc();
    void exprFormatChanged(int );
    void radixChanged(int basedest);
    void reformatShowExpr(const QString&);

    void disableSelectText();
    void showMenu(const QPoint &point);
    void copyResultToClipboard();
    void paste();
    void exprSelectAll();

    QString scanAndExec(int,int,const QString&);

protected:
    virtual void keyPressEvent(QKeyEvent *) override;

protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_programmerHistory;
    int m_currentProgrammerHistoryIndex;
    Session* m_programmerSession;

    QVector<QString> m_numvec;
    QVector<QString> m_opvec;
    QString m_textorder;
    QString m_tmpPreviousExpr;

    SSelection m_selected;
    QList<QString> m_funclist;

    QMenu *m_menu;
    QAction *m_copy;
    QAction *m_paste;
    QAction *m_selectAll;

signals:
    void programmerExprCalcQuantityDec(const Quantity&);
    void programmerExprCalcMessageDec(const QString&);


    void programmerExprCalcMessageHex(const QString&);
    void programmerExprCalcMessageOct(const QString&);
    void programmerExprCalcMessageBin(const QString&);
    void programmerExprCalcError();
    void programmerExprCalcNan();

//    void programmerExprCalcFormatDec(const QString&);
//    void programmerExprCalcFormatHex(const QString&);
//    void programmerExprCalcFormatOct(const QString&);
//    void programmerExprCalcFormatBin(const QString&);

    void programmerToNumConversionFormatDec(const QString&);
    void programmerToNumConversionQuantityDec(const Quantity&);

    void programmerToNumConversionMessageError( );
    void programmerHistoryChanged();
    void programmerEqualPressed();
    void programmerExprIsEmpty();

    void programmerToStageExprFormatDec(const QString&);
    void programmerToStageQuantity(const Quantity&);
    void programmerStageChanged();


};

#endif // PROGRAMMEREXPRCALCULATOR_H
