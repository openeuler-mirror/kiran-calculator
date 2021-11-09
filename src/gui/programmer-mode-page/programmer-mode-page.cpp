/**
* @Copyright (C) 2021 KylinSec Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; If not, see <http: //www.gnu.org/licenses/>.
*
* Author:     luoqing <luoqing@kylinos.com.cn>
*/
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
    ui->programmerExprEdit->setProgrammerKeysPage(ui->programmerKeysPage);
    ui->programmerExprEdit->setStagePage(ui->programmerStagePage);
    ui->programmerHistory->setSession(m_programmerSession);
    ui->programmerClearHistory->setEnabled(false);

    //进制转换默认选中十进制
    ui->numConversion->setCurrentRow(1);

    //计算成功，更新历史记录和暂存记录
    connect(ui->programmerExprEdit, SIGNAL(calculateMode(int)), ui->programmerHistory, SLOT(setCalculateMode(int)));
    connect(ui->programmerExprEdit, SIGNAL(historyChanged( )), ui->programmerHistory, SLOT(updateHistory( )));
    connect(ui->programmerExprEdit, SIGNAL(stageExprFormat(const QString&)), ui->programmerStagePage, SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->programmerExprEdit, SIGNAL(stageQuantity(const Quantity&)), ui->programmerStagePage, SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->programmerExprEdit, SIGNAL(stageChanged( )), ui->programmerStagePage, SLOT(NumFormatStageResult( )));
    connect(ui->programmerExprEdit, SIGNAL(exprCalcError( )), ui->programmerStagePage, SLOT(setStageErrorMessage( )));
    connect(ui->programmerExprEdit, SIGNAL(exprCalcNan( )), ui->programmerStagePage, SLOT(setStageNanMessage( )));

    //清除历史记录
    connect(ui->programmerClearHistory, SIGNAL(clicked()), ui->programmerHistory, SLOT(clearHistory()));

    connect(ui->programmerHistory,&HistoryRecoder::historyClearSuccess, this,[=](){ui->programmerClearHistory->setEnabled(false);});
    connect(ui->programmerExprEdit,&ProgrammerExprCalculator::historyChanged,this,[=](){ui->programmerClearHistory->setEnabled(true);});

    //选中记录
    connect(ui->programmerHistory, SIGNAL(valueSelected(const QString&)), ui->programmerExprEdit, SLOT(setText(const QString& )));
    connect(ui->programmerHistory,SIGNAL(resultSelected(const QString&)), ui->programmerStagePage,SLOT(setHistoryResult(const QString&)));
    connect(ui->programmerStagePage,SIGNAL(stageExprSelected(const QString& )), ui->programmerExprEdit, SLOT(setText(const QString& )));

    //将10进制结果传给进制列表进行转换并显示
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionQuantityDec(const Quantity&)), ui->numConversion,SLOT(showNumFormatConverted(const Quantity&)));
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionMessageError( )), ui->numConversion,SLOT(clearItems( )));
    connect(ui->programmerExprEdit,SIGNAL(programmerExprIsEmpty()),ui->numConversion,SLOT(clearItems()));

    //切换进制时同步刷新输入栏,并限制键盘输入
    connect(ui->numConversion,SIGNAL(numConvered(int )), ui->programmerExprEdit, SLOT(exprFormatChanged(int )));
    connect(ui->numConversion,SIGNAL(numConvered(int )), ui->programmerExprEdit, SLOT(radixChanged(int )));
    //切换进制时限制按钮输入
    connect(ui->numConversion,SIGNAL(numConvered(int)), ui->programmerKeysPage, SLOT(switchProgrammerFormatKeys(int )));

    //进制切换时刷新程序员历史记录
    connect(ui->numConversion,SIGNAL(numConvered(int )), ui->programmerHistory, SLOT(historyFormatChanged(int )));
    connect(ui->numConversion,SIGNAL(refreshNumFormatHistory()), ui->programmerHistory, SLOT(updateHistory()));
    //进制切换时刷新程序员暂存记录
    connect(ui->numConversion,SIGNAL(numConvered(int )), ui->programmerStagePage, SLOT(stageFormatChanged(int )));
    
    connect(ui->numConversion,&NumConversion::refreshNumFormatStage,this,[=](){
        if(!ui->programmerStagePage->text().isEmpty())
        {
            ui->programmerStagePage->NumFormatStageResult();
        }
    });

    connect(ui->programmerKeysPage,SIGNAL(programmerButtonClicked(Button)), this, SLOT(handleProgrammerKeysButtonClicked(Button)));
    connect(ui->programmerKeysPage,SIGNAL(programmerIsShift()), ui->programmerExprEdit,SLOT(handleProgrammerFunction_Shift()));
}

ProgrammerModePage::~ProgrammerModePage()
{
    delete ui;
}

void ProgrammerModePage::handleProgrammerKeysButtonClicked(Button button)
{
    switch (button) {
    case Button_Key_0: ui->programmerExprEdit->handleInsertText("0");break;
    case Button_Key_1: ui->programmerExprEdit->handleInsertText("1");break;
    case Button_Key_2: ui->programmerExprEdit->handleInsertText("2");break;
    case Button_Key_3: ui->programmerExprEdit->handleInsertText("3");break;
    case Button_Key_4: ui->programmerExprEdit->handleInsertText("4");break;
    case Button_Key_5: ui->programmerExprEdit->handleInsertText("5");break;
    case Button_Key_6: ui->programmerExprEdit->handleInsertText("6");break;
    case Button_Key_7: ui->programmerExprEdit->handleInsertText("7");break;
    case Button_Key_8: ui->programmerExprEdit->handleInsertText("8");break;
    case Button_Key_9: ui->programmerExprEdit->handleInsertText("9");break;
    case Button_Key_A: ui->programmerExprEdit->handleInsertText("A");break;
    case Button_Key_B: ui->programmerExprEdit->handleInsertText("B");break;
    case Button_Key_C: ui->programmerExprEdit->handleInsertText("C");break;
    case Button_Key_D: ui->programmerExprEdit->handleInsertText("D");break;
    case Button_Key_E: ui->programmerExprEdit->handleInsertText("E");break;
    case Button_Key_F: ui->programmerExprEdit->handleInsertText("F");break;

    case Button_Key_Add: ui->programmerExprEdit->handleInsertText("+"); break;
    case Button_Key_Sub: ui->programmerExprEdit->handleInsertText("−"); break;
    case Button_Key_Mult: ui->programmerExprEdit->handleInsertText("×"); break;
    case Button_Key_Divide: ui->programmerExprEdit->handleInsertText("÷"); break;

    case Button_Key_And:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleInsertText("0 and");
        else
            ui->programmerExprEdit->handleInsertText("and");
        break;
    case Button_Key_Mod:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleInsertText("0 mod");
        else
            ui->programmerExprEdit->handleInsertText("mod");
        break;
    case Button_Key_Lsh:
        ui->programmerExprEdit->handleProgrammerFunction_Lsh();
        break;
    case Button_Key_Rsh:
        ui->programmerExprEdit->handleProgrammerFunction_Rsh();
        break;
    case Button_Key_Shift:
        ui->programmerKeysPage->switchProgrammerLogicalAndShift();
        break;
    case Button_Key_Or:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleInsertText("0 or");
        else
            ui->programmerExprEdit->handleInsertText("or");
        break;
    case Button_Key_Xor:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleInsertText("0 xor");
        else
            ui->programmerExprEdit->handleInsertText("xor");
        break;
    case Button_Key_Not:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleInsertText("not(0)");
        else
            ui->programmerExprEdit->handleInsertText("not()");
        break;
    case Button_Key_LeftBracket: ui->programmerExprEdit->handleInsertText("("); break;
    case Button_Key_RightBracket: ui->programmerExprEdit->handleInsertText(")"); break;
    case Button_Key_Backspace: ui->programmerExprEdit->handleProgrammerFunction_Backspace(); break;
        //暂时禁用取反
//    case Button_Key_Opposite: ui->programmerExprEdit->handleProgrammerFunction_Opposite(); break;
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
