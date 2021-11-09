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
    ui->scienceExprEdit->autoZoomFontSize();
    ui->scienceHistory->setSession(m_scienceSession);
    ui->scienceExprEdit->setScienceKeysPage(ui->scienceKeysPage);
    ui->scienceExprEdit->setStagePage(ui->scienceStagePage);

    ui->scienceClearHistory->setEnabled(false);

    //键盘按钮点击
    connect(ui->scienceKeysPage, SIGNAL(scienceButtonClicked(Button)), this, SLOT(handleScienceKeysButtonClicked(Button)));
    //当前计算模式传递给历史记录
    connect(ui->scienceExprEdit, SIGNAL(calculateMode(int)), ui->scienceHistory, SLOT(setCalculateMode(int)));

    //将表达式,结果,暂存变动,错误信息传递给暂存记录
    connect(ui->scienceExprEdit,SIGNAL(stageExprFormat(const QString&)),ui->scienceStagePage,SLOT(receiveCalculatedExpr(const QString&)));
    connect(ui->scienceExprEdit,SIGNAL(stageQuantity(const Quantity&)), ui->scienceStagePage,SLOT(receiveCalculatedQuantity(const Quantity&)));
    connect(ui->scienceExprEdit,SIGNAL(stageChanged()),ui->scienceStagePage, SLOT(setStageResult( )));
    connect(ui->scienceExprEdit,SIGNAL(exprCalcError()),ui->scienceStagePage, SLOT(setStageErrorMessage( )));
    connect(ui->scienceExprEdit,SIGNAL(exprCalcNan()),ui->scienceStagePage, SLOT(setStageNanMessage( )));

    //取出历史记录和暂存记录
    connect(ui->scienceHistory,SIGNAL(valueSelected(const QString&)), ui->scienceExprEdit, SLOT(setText(const QString& )));
    connect(ui->scienceHistory,SIGNAL(resultSelected(const QString&)), ui->scienceStagePage,SLOT(setHistoryResult(const QString&)));
    connect(ui->scienceStagePage,SIGNAL(stageExprSelected(const QString&)), ui->scienceExprEdit,SLOT(setText(const QString& )));

    //清除历史记录
    connect(ui->scienceClearHistory, SIGNAL(clicked()), ui->scienceHistory, SLOT(clearHistory()));
    connect(ui->scienceHistory,&HistoryRecoder::historyClearSuccess, this,[=](){ui->scienceClearHistory->setEnabled(false);});
    connect(ui->scienceExprEdit,&ScienceExprCalculator::historyChanged,this,[=](){ui->scienceClearHistory->setEnabled(true);});

    //科学模式下的计算功能切换
    connect(ui->scienceKeysPage,SIGNAL(scienceSwitchAngleUnit(int )), ui->scienceExprEdit,SLOT(handleFunction_SwitchAngleUnit(int )));
    connect(ui->scienceKeysPage,SIGNAL(scienceIsShift()), ui->scienceExprEdit,SLOT(handleFunction_Shift()));
    connect(ui->scienceKeysPage,SIGNAL(scienceIsHYP()), ui->scienceExprEdit,SLOT(handleFunction_HYP()));
    connect(ui->scienceExprEdit,SIGNAL(scienceFEChanged()),ui->scienceHistory,SLOT(historyFEChanged()));
    connect(ui->scienceExprEdit,SIGNAL(scienceFEIndexList(QList<int>)),ui->scienceHistory,SLOT(historyFEIndex(QList<int>)));

    //更新历史记录
    connect(ui->scienceExprEdit, SIGNAL(historyChanged( )), ui->scienceHistory, SLOT(updateHistory( )));
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

void ScienceModePage::handleScienceKeysButtonClicked(Button button)
{
    switch (button) {
    case Button_Key_0: ui->scienceExprEdit->handleInsertText("0"); break;
    case Button_Key_1: ui->scienceExprEdit->handleInsertText("1"); break;
    case Button_Key_2: ui->scienceExprEdit->handleInsertText("2"); break;
    case Button_Key_3: ui->scienceExprEdit->handleInsertText("3"); break;
    case Button_Key_4: ui->scienceExprEdit->handleInsertText("4"); break;
    case Button_Key_5: ui->scienceExprEdit->handleInsertText("5"); break;
    case Button_Key_6: ui->scienceExprEdit->handleInsertText("6"); break;
    case Button_Key_7: ui->scienceExprEdit->handleInsertText("7"); break;
    case Button_Key_8: ui->scienceExprEdit->handleInsertText("8"); break;
    case Button_Key_9: ui->scienceExprEdit->handleInsertText("9"); break;

    case Button_Key_Add: ui->scienceExprEdit->handleInsertText("+"); break;
    case Button_Key_Sub: ui->scienceExprEdit->handleInsertText("−"); break;
    case Button_Key_Mult:
        ui->scienceExprEdit->handleInsertText(QString::fromUtf8("×"));
        break;
    case Button_Key_Divide: ui->scienceExprEdit->handleInsertText("÷"); break;
    case Button_Key_Point: ui->scienceExprEdit->handleInsertText("."); break;
    case Button_Key_LeftBracket: ui->scienceExprEdit->handleInsertText("("); break;
    case Button_Key_RightBracket: ui->scienceExprEdit->handleInsertText(")"); break;
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
            ui->scienceExprEdit->handleInsertText("0mod");
        else
            ui->scienceExprEdit->handleInsertText("mod");
        break;
    case Button_Key_Backspace:ui->scienceExprEdit->handleFunction_Backspace(); break;
    case Button_Key_Pi:
        ui->scienceExprEdit->handleInsertText(QLatin1String("pi"));
//        ui->scienceExprEdit->insert(QString::fromUtf8("π"));
//        ui->scienceExprEdit->handleFunction_Pi();
        break;
    case Button_Key_Factorials:
        ui->scienceExprEdit->handleFunction_Factorials();
        break;
        //暂时禁用取反
//    case Button_Key_Opposite:
//        ui->scienceExprEdit->handleFunction_Opposite();
//        break;
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
