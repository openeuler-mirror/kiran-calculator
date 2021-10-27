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
#ifndef ExprCalculator_H
#define ExprCalculator_H

#include <QWidget>
#include "sselection.h"
#include "core/session.h"
#include <QLineEdit>
#include <QKeyEvent>


class Evaluator;
class Session;
class CNumber;
class QEvent;
class QKeyEvent;
class QWidget;
class HistoryEntry;
class Settings;
class Session;
class StandardKeysPage;

class ExprCalculator : public QLineEdit
{
    Q_OBJECT
public:
    ExprCalculator(QWidget *parent = nullptr);

    void handleFunction_Sqrt();
    void handleFunction_Square();
    void handleFunction_Opposite();
    void handleFunction_Backspace();
    void handleFunction_Reciprocal();

    void setSession(Session*);
    void setKeysPage(StandardKeysPage*);
    bool expressionInFunc(QString &);
    bool judgeInsertPos();     //判断光标位置，禁止在函数名中间插入
    SSelection getSelection() {return m_selected; }
    void setSelection(SSelection select) { m_selected = select; }

    void initMenuAndAction();

protected:
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void evaluate();
    void insert(const QString&);
    void handleInsertText(const QString&);
    void setText(const QString&);
    void exprCalc();
    void reformatShowExpr(const QString&);
    void disableSelectText();

    void showMenu(const QPoint &point);
    void copyResultToClipboard();
    void paste();
    void exprSelectAll();     //selectAll重名了
    void autoZoomFontSize();



protected slots:
    void triggerEnter();

private:
    Evaluator* m_evaluator;
    QList<HistoryEntry> m_standardHistory;
    int m_currentStandardHistoryIndex;
    Session* m_standardSession;
    QString m_savedCurrentEditor;
    QList<QString> m_funclist;
    StandardKeysPage* m_standardKeys;

    SSelection m_selected;


    //输入栏右键菜单
    QMenu *m_menu;
    QAction *m_copy;
    QAction *m_paste;
    QAction *m_selectAll;

signals:
    void exprCalcMessageDec(const QString&);
    void exprCalcQuantityDec(const Quantity&);
    void exprCalcError();
    void exprCalcNan();
    void standardCalculateMode(int );

    void equalPressed();
    void standardHistoryChanged( );

    void standardToStageExprFormat(const QString&);
    void standardToStageQuantity(const Quantity&);
    void standardStageChanged();

};

#endif // ExprCalculator_H
