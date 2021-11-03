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
#include "stage-page.h"
#include "core/evaluator.h"
#include "core/numberformatter.h"
#include "programmer-mode-page/programmer-expr-calculator.h"
#include "utils.h"
#include <QDebug>


int StagePage::m_currentFormat = 1;

#define FORMAT_DEC 1

StagePage::StagePage(QWidget *parent) : QPushButton(parent)
{
    connect(this,SIGNAL(clicked( )),this, SLOT(handleStagePageClicked( )));
    setEnabled(false);
}

void StagePage::stageFormatChanged(int format)
{
    m_currentFormat = format;
}

void StagePage::receiveCalculatedExpr(const QString &expr)
{
    m_saveExpr = expr;
}

void StagePage::receiveCalculatedQuantity(const Quantity &quantity)
{
    m_saveQuantity = quantity;
}

void StagePage::clear()
{
    setText("");
    setEnabled(false);
}

void StagePage::setHistoryResult(const QString & result)
{
    QString stageResult = Utils::reformatSeparators(result);
    setText(stageResult);
    setEnabled(true);
}


void StagePage::NumFormatStageResult()
{
    m_programmerExpr = new ProgrammerExprCalculator();
    QString stageExprConverted;
    stageExprConverted = m_programmerExpr->scanAndExec(FORMAT_DEC,m_currentFormat,m_saveExpr);

    QString expr;
    QString reformatExpr;

    if(!stageExprConverted.isEmpty())
    {
        switch (m_currentFormat) {
        case 0:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 16);
            break;
        case 2:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 8);
            break;
        case 3:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(" "), 2);
            break;
        default:
            expr = stageExprConverted + "=" + DMath::format(m_saveQuantity, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
            reformatExpr = Utils::reformatSeparatorsPro(expr.remove(","), 10);
            break;
        }
        qDebug() << "reformatExpr:" + reformatExpr;
        setText(reformatExpr.replace("-",QString::fromUtf8("−")));
        setEnabled(true);
    }
}

void StagePage::setStageResult()
{
    QString stageList;
    QString stageExpr;
    stageExpr = Utils::reformatSeparators(m_saveExpr);

    if(!m_saveQuantity.isNan())
    {
        stageList.append(stageExpr + "=" + Utils::reformatSeparators(NumberFormatter::format(m_saveQuantity)));
    }
    setText(stageList);
    setEnabled(true);
}

void StagePage::setStageErrorMessage()
{
    QString errorMessage = "表达式错误";
    setText(tr("Expression error"));
    setEnabled(false);
}

void StagePage::setStageNanMessage()
{
    QString nanMessage = "结果无定义";
    setText(tr("The result is undefined"));
    setEnabled(false);
}

void StagePage::handleStagePageClicked( )
{
    QString str = text();
    //以'='号进行分割
    str = str.section('=',-2,-2);
    emit stageExprSelected(str);

}

