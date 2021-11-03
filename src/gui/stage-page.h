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

class Evaluator;
class ProgrammerExprCalculator;

class StagePage : public QPushButton
{
    Q_OBJECT
public:
    StagePage(QWidget *parent=nullptr);
    static int m_currentFormat;


public slots:
    void setStageResult();
    void setStageErrorMessage();
    void setStageNanMessage();
    void setHistoryResult(const QString&);
    void NumFormatStageResult();
    void handleStagePageClicked( );
    void stageFormatChanged(int );
    void receiveCalculatedExpr(const QString&);
    void receiveCalculatedQuantity(const Quantity&);
    void clear();

private:
    Evaluator* m_evaluator;
    ProgrammerExprCalculator* m_programmerExpr;
    QString m_saveExpr;
    Quantity m_saveQuantity;
//    QString m_saveError;

signals:
    void stageExprSelected(const QString &);

};

#endif // STAGEPAGE_H
