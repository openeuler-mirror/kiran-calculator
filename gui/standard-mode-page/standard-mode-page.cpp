#include "standard-mode-page/standard-mode-page.h"
#include "ui_standard-mode-page.h"
#include "core/session.h"
#include "history-recoder.h"
#include "stage-page.h"
#include "standard-mode-page/standard-keys-page.h"
#include "standard-mode-page/expr-calculator.h"

StandardModePage::StandardModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandardModePage)
{
    ui->setupUi(this);

    m_standardSession = new Session();
    ui->standardExprEdit->setSession(m_standardSession);
    ui->standardHistory->setSession(m_standardSession);

    connect(ui->standardKeysPage, SIGNAL(buttonPressed(Button)),this, SLOT(handleStandardKeysButtonPress(Button)));
    connect(ui->standardExprEdit,SIGNAL(standardHistoryChanged( )),ui->standardHistory,SLOT(updateHistory( )));

    connect(ui->standardExprEdit,SIGNAL(standardToStageExprFormat(const QString&)),ui->standardStagePage,SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->standardExprEdit,SIGNAL(standardToStageQuantity(const Quantity&)), ui->standardStagePage,SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->standardExprEdit,SIGNAL(standardStageChanged()),ui->standardStagePage, SLOT(setStageResult( )));

    connect(ui->standardHistory,SIGNAL(exprSelected(const QString&)), ui->standardExprEdit,SLOT(setText(const QString& )));
    connect(ui->standardStagePage,SIGNAL(stageExprSelected(const QString&)), ui->standardExprEdit,SLOT(setText(const QString& )));
    connect(ui->standardClearHistory,SIGNAL(clicked()), ui->standardHistory,SLOT(clearHistory()));

}

void StandardModePage::handleStandardKeysButtonPress(Button button)
{
    switch (button) {
    case Button_Key_0: ui->standardExprEdit->insert("0"); break;
    case Button_Key_1: ui->standardExprEdit->insert("1"); break;
    case Button_Key_2: ui->standardExprEdit->insert("2"); break;
    case Button_Key_3: ui->standardExprEdit->insert("3"); break;
    case Button_Key_4: ui->standardExprEdit->insert("4"); break;
    case Button_Key_5: ui->standardExprEdit->insert("5"); break;
    case Button_Key_6: ui->standardExprEdit->insert("6"); break;
    case Button_Key_7: ui->standardExprEdit->insert("7"); break;
    case Button_Key_8: ui->standardExprEdit->insert("8"); break;
    case Button_Key_9: ui->standardExprEdit->insert("9"); break;

    case Button_Key_Add: ui->standardExprEdit->insert("+"); break;
    case Button_Key_Sub: ui->standardExprEdit->insert("−"); break;
    case Button_Key_Mult: ui->standardExprEdit->insert("×"); break;
    case Button_Key_Divide: ui->standardExprEdit->insert("÷"); break;
    case Button_Key_Point: ui->standardExprEdit->insert("."); break;
    case Button_Key_Brackets:
        ui->standardExprEdit->insert("()");
        ui->standardExprEdit->cursorBackward(false);
        break;
    case Button_Key_Percent:ui->standardExprEdit->insert("%"); break;
    case Button_Key_Sqrt:
        ui->standardExprEdit->handleFunction_Sqrt();
        ui->standardExprEdit->cursorBackward(false);
        break;
    case Button_Key_Equal:
        ui->standardExprEdit->exprCalc();
        break;
    case Button_Key_Square:
        if(ui->standardExprEdit->text().isEmpty())
            ui->standardExprEdit->insert("0^2");
        else
            ui->standardExprEdit->insert("^2");
        break;
    case Button_Key_Reciprocal:
        if(ui->standardExprEdit->text().isEmpty() || ui->standardExprEdit->text() == '0')
            break;
        else
        {
            QString  expr = "1/("+ ui->standardExprEdit->text() + ")";
            ui->standardExprEdit->clear();
            ui->standardExprEdit->insert(expr);
        }
        break;
    case Button_Key_Backspace:
        ui->standardExprEdit->backspace();
        break;
    case Button_Key_Opposite:
        if(ui->standardExprEdit->text().isEmpty())
            return;
        else
            ui->standardExprEdit->handleFunction_Opposite();
        break;
    case Button_Key_ClearEntry: ui->standardExprEdit->clear(); break;
    case Button_Key_Clear:
        ui->standardExprEdit->clear();
        ui->standardStagePage->clear();
        break;
    default: break;
    }
}

void StandardModePage::showEvent(QShowEvent *event)
{
    ui->standardExprEdit->setFocus();
    QWidget::showEvent(event);
}

StandardModePage::~StandardModePage()
{
    delete ui;
}
