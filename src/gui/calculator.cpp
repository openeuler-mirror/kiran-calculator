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

#include <QMessageBox>
#include <qt5-log-i.h>
#include <style-helper.h>
#include <palette.h>

#include "calculator.h"
#include "./ui_calculator.h"
#include "history-recoder.h"
#include "standard-mode-page/standard-keys-page.h"
#include "standard-mode-page/standard-expr-calculator.h"
#include "programmer-mode-page/num-conversion.h"
#include "core/session.h"
#include "core/evaluator.h"
#include "core/settings.h"
#include "general-enum.h"

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), ui(new Ui::Calculator)
{
    ui->setupUi(this);

    m_evaluator = Evaluator::instance();
    m_evaluator->initializeBuiltInVariables();

    ui->stackedWidget->setCurrentIndex(Calculation_Mode_Standard);

    auto palette = Kiran::Theme::Palette::getDefault();
    QObject::connect(palette, &Kiran::Theme::Palette::baseColorsChanged, this, &Calculator::loadStylesheet);

    // loadStylesheet();
}

Calculator::~Calculator()
{
    delete ui;
}

//不同模式下的键盘切换
void Calculator::switchKeyborad(int mode)
{
    ui->stackedWidget->setCurrentIndex(mode);
    if (mode == Calculation_Mode_Standard)
    {
        ui->modeLabel->setText(tr("Standard"));
    }

    if (mode == Calculation_Mode_Science)
    {
        ui->modeLabel->setText(tr("Science"));
    }

    if (mode == Calculation_Mode_Programmer)
    {
        ui->modeLabel->setText(tr("Programmer"));
    }
}

void Calculator::loadStylesheet()
{
    auto style = Kiran::Theme::StyleHelper::getDefault();
    auto paletteType = style->paletteType();
    QString qssPath;
    switch (paletteType)
    {
    case Kiran::Theme::PALETTE_LIGHT:
        qssPath= ":/kiran-calculator-themes/light_theme.qss";
        break ;
    case Kiran::Theme::PALETTE_DARK:
        qssPath= ":/kiran-calculator-themes/black_theme.qss";
        break ;
    default:
        break ;
    }
    QFile file(qssPath);
    if (file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        this->setStyleSheet(style);
    }
    else
    {
        KLOG_INFO() << "load stylesheet failed";
    }
}
