/*
*This file is part of the kiran-calculator project
* Copyright (C) 2021 KylinSec Co., Ltd.
*
* Author: luoqing <luoqing@kylinos.com.cn>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef HISTORYRECODER_H
#define HISTORYRECODER_H

#include <QWidget>
#include <QListWidget>

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


    static int m_currentFormat;
    static int m_currentMode;
    static bool m_FE;

public slots:
    void setCalculateMode(int );
    void updateHistory();

//    void updateScienceHistory();
//    void updateProgrammerHistory();
//    void updateHistoryLabel();

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

private:
    Session* m_session;
    Evaluator* m_evaluator;
    ProgrammerExprCalculator* m_programmerExpr;
    QList<int> m_historyFEIndex;


//    QString m_textorder;
//    QFont m_exprFont;
//    QFont m_resultFont;

signals:
    void resultSelected(const QString &);
    void valueSelected(const QString &);
    void historyClearSuccess();

};

#endif // HISTORYRECODER_H
