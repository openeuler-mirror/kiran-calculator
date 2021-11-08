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
#ifndef EXPRCALCULATOR_H
#define EXPRCALCULATOR_H

#include "core/session.h"
#include <QLineEdit>
class Evaluator;
class Session;
class CNumber;
class HistoryEntry;
class Settings;
class Session;
class StagePage;
class ExprCalculator : public QLineEdit
{
    Q_OBJECT
public:
    ExprCalculator(QWidget *parent = nullptr);

    void setStagePage(StagePage*);
    void setFuncList(QList<QString>);
    void clearStagePage();
    bool expressionInFunc(QString &);
    bool judgeInsertPos();     //判断光标位置，禁止在函数名中间插入

    void handleFunction_Backspace();
    //FIEME:取反函数在多项式取反时有问题,暂时禁用
    void handleFunction_Opposite();
    void handleFunction_Reciprocal();

    
public slots:
    /*TODO:
    1 检测四则运算,加减乘除符号不能连续出现
    2 撤销和返回功能
    3 光标选中删除复制粘贴等
    */
    void insert(const QString&);
    void handleInsertText(const QString&);
    void setText(const QString&);
    void autoZoomFontSize(int fontSize=48);

    //XXX:后续优化格式化函数
    void reformatShowExpr(const QString&);
    void disableSelectText();

signals:
    void exprCalcMessageDec(const QString&);
    void exprCalcMessageHex(const QString&);
    void exprCalcMessageOct(const QString&);
    void exprCalcMessageBin(const QString&);
    void exprCalcQuantityDec(const Quantity&);
    void calculateMode(int );
    void exprCalcError();
    void exprCalcNan();

    void equalPressed();
    void historyChanged( );
    
    void stageExprFormat(const QString&);
    void stageQuantity(const Quantity&);
    void stageChanged();

private:
    Evaluator* m_evaluator;
    Session* m_session;
    QList<QString> m_funclist;
    StagePage* m_stagePage;

};

#endif // EXPRCALCULATOR_H
