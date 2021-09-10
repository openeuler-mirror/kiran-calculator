#include "science-mode-page.h"
#include "ui_science-mode-page.h"
#include "core/session.h"
#include "history-recoder.h"
#include "stage-page.h"
#include "science-mode-page/science-keys-page.h"
#include "science-mode-page/science-expr-calculator.h"
#include "core/settings.h"

ScienceModePage::ScienceModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScienceModePage)
{
    ui->setupUi(this);
    m_scienceSession = new Session();

    ui->scienceExprEdit->setSession(m_scienceSession);
    ui->scienceHistory->setSession(m_scienceSession);

    connect(ui->scienceKeysPage, SIGNAL(scienceButtonPressed(Button)), this, SLOT(handleScienceKeysButtonPress(Button)));
    connect(ui->scienceExprEdit, SIGNAL(scienceHistoryChanged( )), ui->scienceHistory, SLOT(updateScienceHistory( )));

    connect(ui->scienceExprEdit,SIGNAL(scienceToStageExprFormat(const QString&)),ui->scienceStagePage,SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->scienceExprEdit,SIGNAL(scienceToStageQuantity(const Quantity&)), ui->scienceStagePage,SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->scienceExprEdit,SIGNAL(scienceStageChanged()),ui->scienceStagePage, SLOT(setStageResult( )));
    connect(ui->scienceExprEdit,SIGNAL(scienceExprCalcError()),ui->scienceStagePage, SLOT(setStageErrorMessage( )));


    connect(ui->scienceHistory,SIGNAL(exprSelected(const QString&)), ui->scienceExprEdit, SLOT(setText(const QString& )));
    connect(ui->scienceStagePage,SIGNAL(stageExprSelected(const QString&)), ui->scienceExprEdit,SLOT(setText(const QString& )));
    connect(ui->scienceClearHistory, SIGNAL(clicked()), ui->scienceHistory, SLOT(clearScienceHistory()));

    //科学模式下的计算功能切换
    connect(ui->scienceKeysPage,SIGNAL(scienceSwitchAngleUnit(int )), ui->scienceExprEdit,SLOT(handleFunction_SwitchAngleUnit(int )));
    connect(ui->scienceKeysPage,SIGNAL(scienceIsShift()), ui->scienceExprEdit,SLOT(handleFunction_Shift()));
    connect(ui->scienceKeysPage,SIGNAL(scienceIsHYP()), ui->scienceExprEdit,SLOT(handleFunction_HYP()));
    connect(ui->scienceExprEdit,SIGNAL(scienceFEChanged()),ui->scienceHistory,SLOT(historyFEChanged()));


}

ScienceModePage::~ScienceModePage()
{
    delete ui;
}

void ScienceModePage::showEvent(QShowEvent *event)
{
    ui->scienceExprEdit->setFocus();
    QWidget::showEvent(event);
}

void ScienceModePage::handleScienceKeysButtonPress(Button button)
{
    switch (button) {
    case Button_Key_0: ui->scienceExprEdit->insert("0"); break;
    case Button_Key_1: ui->scienceExprEdit->insert("1"); break;
    case Button_Key_2: ui->scienceExprEdit->insert("2"); break;
    case Button_Key_3: ui->scienceExprEdit->insert("3"); break;
    case Button_Key_4: ui->scienceExprEdit->insert("4"); break;
    case Button_Key_5: ui->scienceExprEdit->insert("5"); break;
    case Button_Key_6: ui->scienceExprEdit->insert("6"); break;
    case Button_Key_7: ui->scienceExprEdit->insert("7"); break;
    case Button_Key_8: ui->scienceExprEdit->insert("8"); break;
    case Button_Key_9: ui->scienceExprEdit->insert("9"); break;

    case Button_Key_Add: ui->scienceExprEdit->insert("+"); break;
    case Button_Key_Sub: ui->scienceExprEdit->insert("−"); break;
    case Button_Key_Mult:
//        ui->scienceExprEdit->insert("×");
        ui->scienceExprEdit->insert(QString::fromUtf8("×"));
        break;
    case Button_Key_Divide: ui->scienceExprEdit->insert("÷"); break;
    case Button_Key_Point: ui->scienceExprEdit->insert("."); break;
    case Button_Key_Brackets:
        ui->scienceExprEdit->insert("()");
        ui->scienceExprEdit->cursorBackward(false);
        break;
    case Button_Key_Square:
        ui->scienceExprEdit->handleFunction_Square();
        break;
    case Button_Key_Xn:
        ui->scienceExprEdit->handleFunction_Xn();
        break;
    case Button_Key_Sin:
        ui->scienceExprEdit->handleFunction_Sin();
        break;
    case Button_Key_Cos:
        ui->scienceExprEdit->handleFunction_Cos();
        break;
    case Button_Key_Tan:
        ui->scienceExprEdit->handleFunction_Tan();
        break;
    case Button_Key_Sqrt:
        ui->scienceExprEdit->handleFunction_Sqrt();
        ui->scienceExprEdit->cursorBackward(false);
        break;
    case Button_Key_10n:
        ui->scienceExprEdit->handleFunction_10n();
        break;
    case Button_Key_Log:
        ui->scienceExprEdit->handleFunction_Log();
        break;
    case Button_Key_Exp:
        ui->scienceExprEdit->handleFunction_Exp();
        break;
    case Button_Key_Mod:
        if(ui->scienceExprEdit->text().isEmpty())
            ui->scienceExprEdit->insert("0mod");
        else
            ui->scienceExprEdit->insert("mod");
        break;
    case Button_Key_Backspace:ui->scienceExprEdit->handleFunction_Backspace(); break;
    case Button_Key_Pi:
        ui->scienceExprEdit->insert(QLatin1String("pi"));
//        ui->scienceExprEdit->insert(QString::fromUtf8("π"));
//        ui->scienceExprEdit->handleFunction_Pi();
        break;
    case Button_Key_Factorials:
        ui->scienceExprEdit->handleFunction_Factorials();
        break;
    case Button_Key_Opposite:
        ui->scienceExprEdit->handleFunction_Opposite();
        break;
    case Button_Key_Equal:
        ui->scienceExprEdit->scienceExprCalc();
        break;
    case Button_Key_ClearEntry: ui->scienceExprEdit->clear(); break;
    case Button_Key_Clear:
        ui->scienceExprEdit->clear();
        ui->scienceStagePage->clear();
        break;
    case Button_Key_AngleUnit:
        ui->scienceKeysPage->switchScienceAngleUnit();
        break;
    case Button_Key_HYP:
        ui->scienceKeysPage->switchScienceHYP();
        break;
    case Button_Key_FE:
        ui->scienceExprEdit->handleFunction_FE();
        break;
    case Button_Key_Shift:
        ui->scienceKeysPage->shiftScienceFunction();
        break;
    default: break;
    }
}
