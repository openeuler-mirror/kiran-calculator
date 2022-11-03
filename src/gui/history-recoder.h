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
class QLabel;
class QVBoxLayout;

class HistoryRecoder : public QListWidget
{
    Q_OBJECT
public:
    explicit HistoryRecoder(QWidget *parent = nullptr);
    void setSession(Session* session);

public slots:
    //TODO:历史记录右键点击功能
    void setCalculateMode(int mode);
    void updateHistory();
    void historyFEChanged();
    void historyFEIndex(QList<int > index);
    void historyFormatChanged(int format);
    void clearHistory();
    bool isHistoryEmpty();
    QString historyWordWrap(const QString& text, int limitLength);

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                         const QEvent *event) const override;
protected slots:
    void handleItem(QListWidgetItem *item);

signals:
    void resultSelected(const QString &result);
    void valueSelected(const QString &value);
    void historyClearSuccess();

private:
    Session* m_session;
    Evaluator* m_evaluator;
    QList<int> m_historyFEIndex;

    QList<QLabel *> m_labelExprList;
    QList<QLabel *> m_labelValueList;
    QList<QVBoxLayout *> m_vboxlayoutList;
    QList<QWidget *>m_historyWidgetList;
    QList<QListWidgetItem *>m_historyItemList;

    int m_currentFormat = Num_Format_Dec;
    int m_currentMode = Calculation_Mode_Standard;
    bool m_FE = false; //科学计算法模式默认关闭
};

#endif // HISTORYRECODER_H
