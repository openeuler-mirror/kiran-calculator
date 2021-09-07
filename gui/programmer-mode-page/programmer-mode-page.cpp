#include "programmer-mode-page.h"
#include "ui_programmer-mode-page.h"

#include "core/session.h"
#include "history-recoder.h"
#include "stage-page.h"
#include "programmer-mode-page/programmer-expr-calculator.h"
#include "programmer-mode-page/num-conversion.h"

#include <QDebug>

ProgrammerModePage::ProgrammerModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgrammerModePage)
{
    ui->setupUi(this);

    m_programmerSession = new Session();
    ui->programmerExprEdit->setSession(m_programmerSession);
    ui->programmerHistory->setSession(m_programmerSession);


    //计算成功，更新历史记录和暂存记录
    connect(ui->programmerExprEdit, SIGNAL(programmerHistoryChanged( )), ui->programmerHistory, SLOT(NumFormateHistory( )));
    connect(ui->programmerExprEdit, SIGNAL(programmerToStageExprFormatDec(const QString&)), ui->programmerStagePage, SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->programmerExprEdit, SIGNAL(programmerToStageQuantity(const Quantity&)), ui->programmerStagePage, SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->programmerExprEdit, SIGNAL(programmerStageChanged( )), ui->programmerStagePage, SLOT(NumFormatStageResult( )));

    //清除历史记录
    connect(ui->programmerClearHistory, SIGNAL(clicked()), ui->programmerHistory, SLOT(clearProgrammerHistory()));

    //选中记录
    connect(ui->programmerHistory, SIGNAL(exprSelected(const QString&)), ui->programmerExprEdit, SLOT(setText(const QString& )));
    connect(ui->programmerStagePage,SIGNAL(stageExprSelected(const QString& )), ui->programmerExprEdit, SLOT(setText(const QString& )));

    //将10进制结果传给进制列表进行转换并显示
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionQuantityDec(const Quantity&)), ui->numConversionTable,SLOT(showNumFormatConverted(const Quantity&)));
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionMessageError( )), ui->numConversionTable,SLOT(clearItems( )));
    connect(ui->programmerExprEdit,SIGNAL(programmerExprIsEmpty()),ui->numConversionTable,SLOT(clearItems()));

    //切换进制时同步刷新输入栏,并限制键盘输入
    connect(ui->numConversionTable,SIGNAL(numConvered(int )), ui->programmerExprEdit, SLOT(exprFormatChanged(int )));
    connect(ui->numConversionTable,SIGNAL(numConvered(int )), ui->programmerExprEdit, SLOT(radixChanged(int )));
    //切换进制时限制按钮输入
    connect(ui->numConversionTable,SIGNAL(numConvered(int)), ui->programmerKeysPage, SLOT(switchProgrammerFormatKeys(int )));

    //进制切换时刷新程序员历史记录
    connect(ui->numConversionTable,SIGNAL(numConvered(int )), ui->programmerHistory, SLOT(historyFormatChanged(int )));
    connect(ui->numConversionTable,SIGNAL(refreshNumFormatHistory()), ui->programmerHistory, SLOT(NumFormateHistory()));
    //进制切换时刷新程序员暂存记录
    connect(ui->numConversionTable,SIGNAL(numConvered(int )), ui->programmerStagePage, SLOT(stageFormatChanged(int )));
    connect(ui->numConversionTable,SIGNAL(refreshNumFormatStage()), ui->programmerStagePage, SLOT(NumFormatStageResult( )));

    //
    connect(ui->programmerKeysPage,SIGNAL(programmerButtonPressed(Button)), this, SLOT(handleProgrammerKeysButtonPress(Button)));

    //设置进制转换列表不可垂直滚动
    ui->numConversionTable->verticalScrollBar()->setDisabled(true);
    //行宽自适应划分整个控件的高度,固定不可拖动
    ui->numConversionTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //默认选中十进制
    ui->numConversionTable->selectRow(1);

}

ProgrammerModePage::~ProgrammerModePage()
{
    delete ui;
}

void ProgrammerModePage::handleProgrammerKeysButtonPress(Button button)
{
//    ui->programmerExprEdit->setFocus();
    switch (button) {
    case Button_Key_0:

        ui->programmerExprEdit->insert("0");break;
    case Button_Key_1: ui->programmerExprEdit->insert("1");break;
    case Button_Key_2: ui->programmerExprEdit->insert("2");break;
    case Button_Key_3: ui->programmerExprEdit->insert("3");break;
    case Button_Key_4: ui->programmerExprEdit->insert("4");break;
    case Button_Key_5: ui->programmerExprEdit->insert("5");break;
    case Button_Key_6: ui->programmerExprEdit->insert("6");break;
    case Button_Key_7: ui->programmerExprEdit->insert("7");break;
    case Button_Key_8: ui->programmerExprEdit->insert("8");break;
    case Button_Key_9: ui->programmerExprEdit->insert("9");break;
    case Button_Key_A: ui->programmerExprEdit->insert("A");break;
    case Button_Key_B: ui->programmerExprEdit->insert("B");break;
    case Button_Key_C: ui->programmerExprEdit->insert("C");break;
    case Button_Key_D: ui->programmerExprEdit->insert("D");break;
    case Button_Key_E: ui->programmerExprEdit->insert("E");break;
    case Button_Key_F: ui->programmerExprEdit->insert("F");break;

    case Button_Key_Add: ui->programmerExprEdit->insert("+"); break;
    case Button_Key_Sub: ui->programmerExprEdit->insert("−"); break;
    case Button_Key_Mult: ui->programmerExprEdit->insert("×"); break;
    case Button_Key_Divide: ui->programmerExprEdit->insert("÷"); break;

    case Button_Key_And:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 and");
        else
            ui->programmerExprEdit->insert("and");
        break;
    case Button_Key_Mod:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 mod");
        else
            ui->programmerExprEdit->insert("mod");
        break;
    case Button_Key_Lsh:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 shl");
        else
            ui->programmerExprEdit->insert("shl");
        break;
    case Button_Key_Rsh:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 shr");
        else
            ui->programmerExprEdit->insert("shr");
        break;
    case Button_Key_RoL:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 rol");
        else
            ui->programmerExprEdit->insert("rol");
        break;
        break;
    case Button_Key_RoR:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 ror");
        else
            ui->programmerExprEdit->insert("ror");
        break;
    case Button_Key_Shift:
        ui->programmerKeysPage->switchProgrammerLogicalAndShift();
        break;
    case Button_Key_Or:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 or");
        else
            ui->programmerExprEdit->insert("or");
        break;
    case Button_Key_Xor:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("0 xor");
        else
            ui->programmerExprEdit->insert("xor");
        break;
    case Button_Key_Not:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->insert("not(0)");
        else
            ui->programmerExprEdit->insert("not()");
        break;

    case Button_Key_Brackets:
        ui->programmerExprEdit->insert("()");
        ui->programmerExprEdit->cursorBackward(false);
        break;
    case Button_Key_Backspace: ui->programmerExprEdit->handleProgrammerFunction_Backspace(); break;
    case Button_Key_Opposite: ui->programmerExprEdit->handleProgrammerFunction_Opposite(); break;
    case Button_Key_Equal: ui->programmerExprEdit->programmerExprCalc(); break;
    case Button_Key_ClearEntry: ui->programmerExprEdit->clear(); break;
    case Button_Key_Clear:
        ui->programmerExprEdit->clear();
        ui->programmerStagePage->clear();
        break;
    default: break;
    }
}

void ProgrammerModePage::showEvent(QShowEvent *event){
    ui->programmerExprEdit->setFocus();
    QWidget::showEvent(event);
}
