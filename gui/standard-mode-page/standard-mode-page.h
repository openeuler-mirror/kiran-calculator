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
#ifndef STANDARDMODEPAGE_H
#define STANDARDMODEPAGE_H

#include "keys-page.h"
#include <QWidget>

class Session;
class StandardKeysPage;
class ExprCalculator;
class HistoryRecoder;
class StagePage;
enum Button;
namespace Ui {
class StandardModePage;
}

class StandardModePage : public QWidget
{
    Q_OBJECT

public:
    explicit StandardModePage(QWidget *parent = nullptr);
    ~StandardModePage();


public slots:
    void handleStandardKeysButtonPress(Button);

protected:
    virtual void showEvent(QShowEvent *event) override;
private:
    Ui::StandardModePage *ui;
    Session *m_standardSession = nullptr;
};

#endif // STANDARDMODEPAGE_H
