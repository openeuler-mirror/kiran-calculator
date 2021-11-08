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

#ifndef HISTORYRECODER_H
#define HISTORYRECODER_H

#include <QWidget>
#include <QListWidget>
#include "general-enum.h"

class QListWidget;
class QListWidgetItem;
class HistoryEntry;
class Evaluator;
class Session;
class ProgrammerExprCalculator;

class HistoryRecoder : public QListWidget
{
    Q_OBJECT
public:
    explicit HistoryRecoder(QWidget *parent = nullptr);
    void setSession(Session*);

public slots:
    //TODO:历史记录右键点击功能
    void setCalculateMode(int );
    void updateHistory();
    void historyFEChanged();
    void historyFEIndex(QList<int >);
    void historyFormatChanged(int );
    void clearHistory();
    bool isHistoryEmpty();
    QString historyWordWrap(const QString&, int );

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                         const QEvent *event) const override;
protected slots:
    void handleItem(QListWidgetItem *);

signals:
    void resultSelected(const QString &);
    void valueSelected(const QString &);
    void historyClearSuccess();

private:
    Session* m_session;
    Evaluator* m_evaluator;
    ProgrammerExprCalculator* m_programmerExpr;
    QList<int> m_historyFEIndex;

    int m_currentFormat = Num_Format_Dec;
    int m_currentMode = Calculation_Mode_Standard;
    bool m_FE = false; //科学计算法模式默认关闭
};

#endif // HISTORYRECODER_H
