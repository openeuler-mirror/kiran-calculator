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
#ifndef PROGRAMMERMODEPAGE_H
#define PROGRAMMERMODEPAGE_H

#include <QWidget>
#include <QScrollBar>
#include "keys-page.h"
class Session;
class HistoryRecoder;
class StagePage;
class ProgrammerExprCalculator;
class NumConversion;
class NumConversionModel;
enum Button;
namespace Ui {
class ProgrammerModePage;
}

class ProgrammerModePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProgrammerModePage(QWidget *parent = nullptr);
    ~ProgrammerModePage();

public slots:
    void handleProgrammerKeysButtonPress(Button);

protected:
    virtual void showEvent(QShowEvent *event) override;
private:
    Ui::ProgrammerModePage *ui;
    Session *m_programmerSession = nullptr;
};

#endif // PROGRAMMERMODEPAGE_H
