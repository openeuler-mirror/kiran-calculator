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
#include "standard-mode-page/standard-mode-page.h"
#include "ui_standard-mode-page.h"
#include "core/session.h"
#include "history-recoder.h"
#include "stage-page.h"
#include "standard-mode-page/standard-keys-page.h"
#include "standard-mode-page/standard-expr-calculator.h"


StandardModePage::StandardModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StandardModePage)
{
    ui->setupUi(this);

    ui->standardExprEdit->autoZoomFontSize();       //设置好字体大小以及光标的大小，防止初始化时光标大小不一致

    m_standardSession = new Session();
    ui->standardExprEdit->setSession(m_standardSession);
    ui->standardHistory->setSession(m_standardSession);
    ui->standardExprEdit->setStandardKeysPage(ui->standardKeysPage);
    ui->standardExprEdit->setStagePage(ui->standardStagePage);

    ui->standardClearHistory->setEnabled(false);

    //键盘按钮点击
    connect(ui->standardKeysPage, SIGNAL(buttonClicked(Button)),this, SLOT(handleStandardKeysButtonClicked(Button)));
    //传递当前模式
    connect(ui->standardExprEdit,SIGNAL(calculateMode(int )),ui->standardHistory,SLOT(setCalculateMode(int )));
    //更新历史记录
    connect(ui->standardExprEdit,SIGNAL(historyChanged( )),ui->standardHistory,SLOT(updateHistory( )));
    //将表达式,结果,暂存变动,错误信息传递给暂存记录
    connect(ui->standardExprEdit,SIGNAL(stageExprFormat(const QString&)),ui->standardStagePage,SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->standardExprEdit,SIGNAL(stageQuantity(const Quantity&)), ui->standardStagePage,SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->standardExprEdit,SIGNAL(stageChanged()),ui->standardStagePage, SLOT(setStageResult( )));
    connect(ui->standardExprEdit,SIGNAL(exprCalcError()),ui->standardStagePage, SLOT(setStageErrorMessage( )));
    connect(ui->standardExprEdit,SIGNAL(exprCalcNan()),ui->standardStagePage, SLOT(setStageNanMessage( )));
    //取出历史记录和暂存记录
    connect(ui->standardHistory,SIGNAL(valueSelected(const QString&)), ui->standardExprEdit,SLOT(setText(const QString& )));
    connect(ui->standardHistory,SIGNAL(resultSelected(const QString&)), ui->standardStagePage,SLOT(setHistoryResult(const QString&)));
    connect(ui->standardStagePage,SIGNAL(stageExprSelected(const QString&)), ui->standardExprEdit,SLOT(setText(const QString& )));
    //清除历史记录
    connect(ui->standardClearHistory,SIGNAL(clicked()), ui->standardHistory,SLOT(clearHistory()));
    connect(ui->standardHistory,&HistoryRecoder::historyClearSuccess, this,[=](){ui->standardClearHistory->setEnabled(false);});
    connect(ui->standardExprEdit,&StandardExprCalculator::historyChanged,this,[=](){ui->standardClearHistory->setEnabled(true);});
}

void StandardModePage::handleStandardKeysButtonClicked(Button button)
{
    switch (button) {
    case Button_Key_0: ui->standardExprEdit->handleInsertText("0"); break;
    case Button_Key_1: ui->standardExprEdit->handleInsertText("1"); break;
    case Button_Key_2: ui->standardExprEdit->handleInsertText("2"); break;
    case Button_Key_3: ui->standardExprEdit->handleInsertText("3"); break;
    case Button_Key_4: ui->standardExprEdit->handleInsertText("4"); break;
    case Button_Key_5: ui->standardExprEdit->handleInsertText("5"); break;
    case Button_Key_6: ui->standardExprEdit->handleInsertText("6"); break;
    case Button_Key_7: ui->standardExprEdit->handleInsertText("7"); break;
    case Button_Key_8: ui->standardExprEdit->handleInsertText("8"); break;
    case Button_Key_9: ui->standardExprEdit->handleInsertText("9"); break;

    case Button_Key_Add: ui->standardExprEdit->handleInsertText("+"); break;
    case Button_Key_Sub: ui->standardExprEdit->handleInsertText("−"); break;
    case Button_Key_Mult: ui->standardExprEdit->handleInsertText("×"); break;
    case Button_Key_Divide: ui->standardExprEdit->handleInsertText("÷"); break;
    case Button_Key_Point: ui->standardExprEdit->handleInsertText("."); break;
    case Button_Key_Brackets:
        ui->standardExprEdit->handleInsertText("()");
        ui->standardExprEdit->cursorBackward(false);
        break;
    case Button_Key_Percent:ui->standardExprEdit->handleInsertText("%"); break;
    case Button_Key_Sqrt:
        ui->standardExprEdit->handleFunction_Sqrt();
        break;
    case Button_Key_Equal:
        ui->standardExprEdit->exprCalc();
        break;
    case Button_Key_Square:
        ui->standardExprEdit->handleFunction_Square();
        break;
    case Button_Key_Reciprocal:
        ui->standardExprEdit->handleFunction_Reciprocal();
        break;
    case Button_Key_Backspace:
        ui->standardExprEdit->handleFunction_Backspace();
        break;
        //暂时禁用取反
//    case Button_Key_Opposite:
//        if(ui->standardExprEdit->text().isEmpty())
//            return;
//        else
//            ui->standardExprEdit->handleFunction_Opposite();
//        break;
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
