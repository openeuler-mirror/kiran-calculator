#ifndef PROGRAMMEREXPRCALCULATOR_H
#define PROGRAMMEREXPRCALCULATOR_H
#include <QLineEdit>
#include <QWidget>
#include "core/session.h"
#include "core/sessionhistory.h"
class Evaluator;
class Session;
class CNumber;
class HistoryEntry;
class Settings;

struct SSelection {
    QString oldText; //选中时的当前text
    QString selected; //被选中部分
    QString clearText;
    bool isChanged;
    int curpos = 0;
    SSelection() { isChanged = false; }
};


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

    static bool isNumber(QChar a);//判断是否为数字(分隔符)
    static int  m_currentFormat;
    static int  m_previousFormat;
    static bool m_numOutRange;
    enum NumFormat{
        Num_Format_Hex,Num_Format_Dec,Num_Format_Oct,Num_Format_Bin
    };

    SSelection getSelection() { return m_selected; }

public slots:
    void insert(const QString&);
    void setText(const QString&);
    void programmerExprCalc();
    void autoProgrammerExprCalc();
    void exprFormatChanged(int );
    void radixChanged(int basedest);

    QString scanAndExec(int,int,const QString&);

    bool isNumberOutOfRange(const QString&);

    void reformatShowExpr(const QString&);

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
    QList<QString> m_funclist; //支持的函数

signals:
    void programmerExprCalcQuantityDec(const Quantity&);
    void programmerExprCalcMessageDec(const QString&);


    void programmerExprCalcMessageHex(const QString&);
    void programmerExprCalcMessageOct(const QString&);
    void programmerExprCalcMessageBin(const QString&);

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
