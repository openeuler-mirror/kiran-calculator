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
#ifndef NUMCONVERSION_H
#define NUMCONVERSION_H
#include <QWidget>
#include <QListWidget>
#include "core/session.h"
#include "core/sessionhistory.h"
#include <QEvent>
#include <QMouseEvent>
class Evaluator;
class Session;
class QString;
class NumConversion : public QListWidget
{
    Q_OBJECT
public:
    NumConversion(QWidget* parent = nullptr);

public slots:
    void showNumFormatConverted(const Quantity& quantity);
    void clearItems();
    void activateNumConversion(QListWidgetItem* current,QListWidgetItem* previous);

signals:
    void numConvered(int currentRow);
    void refreshNumFormatHistory( );
    void refreshNumFormatStage();

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index,
                                                         const QEvent *event) const override;

private:
    Evaluator* m_evaluator;
    Session* m_programmerSession;
    QString m_messageDec;
};

#endif // NUMCONVERSION_H
