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
#include "programmer-mode-page/num-conversion-model.h"
#include "programmer-mode-page/num-conversion-view.h"

#include <QDebug>

ProgrammerModePage::ProgrammerModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgrammerModePage)
{
    ui->setupUi(this);

//    NumConversionModel *numConversionModel = new NumConversionModel(this);
//    ui->numConversionView->setModel(numConversionModel);
//    ui->numConversionView->horizontalHeader()->hide();

    m_programmerSession = new Session();
    ui->programmerExprEdit->setSession(m_programmerSession);
    ui->programmerHistory->setSession(m_programmerSession);
    ui->programmerClearHistory->setEnabled(false);

//    ui->numConversion->item(0)->setSizeHint(QSize(332,24));

//    ui->numConversion->horizontalHeader()->hide();
//    ui->numConversion->verticalHeaderItem(0)->setSizeHint(QSize(64,24));
//    ui->numConversion->verticalHeader()->setStretchLastSection(true);
    //行宽自适应划分整个控件的高度,固定不可拖动
//    ui->numConversion->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->numConversion->adjustSize();
    //默认选中十进制
//    ui->numConversion->selectRow(1);


    //计算成功，更新历史记录和暂存记录
    connect(ui->programmerExprEdit, SIGNAL(programmerCalculateMode(int)), ui->programmerHistory, SLOT(setCalculateMode(int)));
    connect(ui->programmerExprEdit, SIGNAL(programmerHistoryChanged( )), ui->programmerHistory, SLOT(updateHistory( )));
    connect(ui->programmerExprEdit, SIGNAL(programmerToStageExprFormatDec(const QString&)), ui->programmerStagePage, SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->programmerExprEdit, SIGNAL(programmerToStageQuantity(const Quantity&)), ui->programmerStagePage, SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->programmerExprEdit, SIGNAL(programmerStageChanged( )), ui->programmerStagePage, SLOT(NumFormatStageResult( )));
    connect(ui->programmerExprEdit, SIGNAL(programmerExprCalcError( )), ui->programmerStagePage, SLOT(setStageErrorMessage( )));
    connect(ui->programmerExprEdit, SIGNAL(programmerExprCalcNan( )), ui->programmerStagePage, SLOT(setStageNanMessage( )));

    //清除历史记录
    connect(ui->programmerClearHistory, SIGNAL(clicked()), ui->programmerHistory, SLOT(clearHistory()));

    connect(ui->programmerHistory,&HistoryRecoder::historyClearSuccess, this,[=](){ui->programmerClearHistory->setEnabled(false);});
    connect(ui->programmerExprEdit,&ProgrammerExprCalculator::programmerHistoryChanged,this,[=](){ui->programmerClearHistory->setEnabled(true);});

    //选中记录
    connect(ui->programmerHistory, SIGNAL(valueSelected(const QString&)), ui->programmerExprEdit, SLOT(setText(const QString& )));
    connect(ui->programmerStagePage,SIGNAL(stageExprSelected(const QString& )), ui->programmerExprEdit, SLOT(setText(const QString& )));

    //将10进制结果传给进制列表进行转换并显示
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionQuantityDec(const Quantity&)), ui->numConversion,SLOT(showNumFormatConverted(const Quantity&)));
    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionMessageError( )), ui->numConversion,SLOT(clearItems( )));
    connect(ui->programmerExprEdit,SIGNAL(programmerExprIsEmpty()),ui->numConversion,SLOT(clearItems()));


//    connect(ui->programmerExprEdit,SIGNAL(programmerToNumConversionQuantityDec(const Quantity&)), numConversionModel,SLOT(showNumFormatConverted(const Quantity&)));


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

    connect(ui->numConversion,SIGNAL(refreshNumFormatStage()), ui->programmerStagePage, SLOT(NumFormatStageResult( )));


    //
    connect(ui->programmerKeysPage,SIGNAL(programmerButtonPressed(Button)), this, SLOT(handleProgrammerKeysButtonPress(Button)));
    connect(ui->programmerKeysPage,SIGNAL(programmerIsShift()), ui->programmerExprEdit,SLOT(handleProgrammerFunction_Shift()));

}

ProgrammerModePage::~ProgrammerModePage()
{
    delete ui;
}

void ProgrammerModePage::handleProgrammerKeysButtonPress(Button button)
{
//    ui->programmerExprEdit->setFocus();
    switch (button) {
    case Button_Key_0: ui->programmerExprEdit->handleProgrammerInsertText("0");break;
    case Button_Key_1: ui->programmerExprEdit->handleProgrammerInsertText("1");break;
    case Button_Key_2: ui->programmerExprEdit->handleProgrammerInsertText("2");break;
    case Button_Key_3: ui->programmerExprEdit->handleProgrammerInsertText("3");break;
    case Button_Key_4: ui->programmerExprEdit->handleProgrammerInsertText("4");break;
    case Button_Key_5: ui->programmerExprEdit->handleProgrammerInsertText("5");break;
    case Button_Key_6: ui->programmerExprEdit->handleProgrammerInsertText("6");break;
    case Button_Key_7: ui->programmerExprEdit->handleProgrammerInsertText("7");break;
    case Button_Key_8: ui->programmerExprEdit->handleProgrammerInsertText("8");break;
    case Button_Key_9: ui->programmerExprEdit->handleProgrammerInsertText("9");break;
    case Button_Key_A: ui->programmerExprEdit->handleProgrammerInsertText("A");break;
    case Button_Key_B: ui->programmerExprEdit->handleProgrammerInsertText("B");break;
    case Button_Key_C: ui->programmerExprEdit->handleProgrammerInsertText("C");break;
    case Button_Key_D: ui->programmerExprEdit->handleProgrammerInsertText("D");break;
    case Button_Key_E: ui->programmerExprEdit->handleProgrammerInsertText("E");break;
    case Button_Key_F: ui->programmerExprEdit->handleProgrammerInsertText("F");break;

    case Button_Key_Add: ui->programmerExprEdit->handleProgrammerInsertText("+"); break;
    case Button_Key_Sub: ui->programmerExprEdit->handleProgrammerInsertText("−"); break;
    case Button_Key_Mult: ui->programmerExprEdit->handleProgrammerInsertText("×"); break;
    case Button_Key_Divide: ui->programmerExprEdit->handleProgrammerInsertText("÷"); break;

    case Button_Key_And:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleProgrammerInsertText("0 and");
        else
            ui->programmerExprEdit->handleProgrammerInsertText("and");
        break;
    case Button_Key_Mod:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleProgrammerInsertText("0 mod");
        else
            ui->programmerExprEdit->handleProgrammerInsertText("mod");
        break;
    case Button_Key_Lsh:
//        if(ui->programmerExprEdit->text().isEmpty())
//            ui->programmerExprEdit->handleProgrammerInsertText("0 shl");
//        else
//            ui->programmerExprEdit->handleProgrammerInsertText("shl");
        ui->programmerExprEdit->handleProgrammerFunction_Lsh();
        break;
    case Button_Key_Rsh:
//        if(ui->programmerExprEdit->text().isEmpty())
//            ui->programmerExprEdit->handleProgrammerInsertText("0 shr");
//        else
//            ui->programmerExprEdit->handleProgrammerInsertText("shr");
        ui->programmerExprEdit->handleProgrammerFunction_Rsh();
        break;
    case Button_Key_Shift:
        ui->programmerKeysPage->switchProgrammerLogicalAndShift();
        break;
    case Button_Key_Or:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleProgrammerInsertText("0 or");
        else
            ui->programmerExprEdit->handleProgrammerInsertText("or");
        break;
    case Button_Key_Xor:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleProgrammerInsertText("0 xor");
        else
            ui->programmerExprEdit->handleProgrammerInsertText("xor");
        break;
    case Button_Key_Not:
        if(ui->programmerExprEdit->text().isEmpty())
            ui->programmerExprEdit->handleProgrammerInsertText("not(0)");
        else
            ui->programmerExprEdit->handleProgrammerInsertText("not()");
        break;
    case Button_Key_LeftBracket: ui->programmerExprEdit->handleProgrammerInsertText("("); break;
    case Button_Key_RightBracket: ui->programmerExprEdit->handleProgrammerInsertText(")"); break;
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
