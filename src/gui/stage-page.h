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
#ifndef STAGEPAGE_H
#define STAGEPAGE_H
#include "quantity.h"
#include <QListWidget>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
#include "general-enum.h"


class ProgrammerExprCalculator;

class StagePage : public QPushButton
{
    Q_OBJECT
public:
    StagePage(QWidget *parent=nullptr);
    
public slots:
    void setStageResult();
    void setStageErrorMessage();
    void setStageNanMessage();
    void setHistoryResult(const QString& result);
    void NumFormatStageResult();
    void handleStagePageClicked( );
    void stageFormatChanged(int format);
    void receiveCalculatedExpr(const QString& expr);
    void receiveCalculatedQuantity(const Quantity& quantity);
    void clear();

signals:
    void stageExprSelected(const QString &expr);

private:
    ProgrammerExprCalculator* m_programmerExpr;
    QString m_saveExpr;
    Quantity m_saveQuantity;

    int m_currentFormat = Num_Format_Dec;
};

#endif // STAGEPAGE_H
