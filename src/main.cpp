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
#include "gui/calculator.h"
#include <QDebug>
#include <QApplication>
#include <kiran-titlebar-window.h>
#include <qt5-log-i.h>
#include <QTranslator>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "gui/mode-selection.h"
#include "config.h"
#include <kiran-application.h>

void loadStylesheet()
{
    QFile file(":/kiran-calculator-themes/black_theme.qss");
    if(file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
    }
    else
    {
        qDebug() << "load stylesheet failed";
    }
}

int main(int argc, char *argv[])
{
    KiranApplication a(argc, argv);
    klog_qt5_init("", "kylinsec-session", "kiran-calculator", "calculator");

    QTranslator translator;
    if(translator.load(QLocale(),"kiran-calculator",".",TRANSLATION_DIR_PATH,".qm"))
    {
        a.installTranslator(&translator);
        KLOG_INFO() << "installTranslator load:" << a.installTranslator(&translator);
    }
    loadStylesheet();
    KiranTitlebarWindow window;
    window.setTitle(QObject::tr("Calculator"));
    window.setIcon(QIcon(CALCULATOR_ICON_PATH"/kc-calculator.png"));
    window.setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    window.setResizeable(false);


    Calculator calculator;
    auto* mode = new ModeSelection(&calculator);
    mode->setObjectName("modeButton");

    auto spaceItemLeft = new QSpacerItem(20,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    window.getTitlebarCustomLayout()->addItem(spaceItemLeft);

    window.setTitlebarCustomLayoutAlignHCenter(false);
    window.getTitlebarCustomLayout()->addWidget(mode,Qt::AlignRight);

    auto spaceItemRight = new QSpacerItem(20,20,QSizePolicy::Fixed,QSizePolicy::Minimum);
    window.getTitlebarCustomLayout()->addItem(spaceItemRight);

    window.setWindowContentWidget(&calculator);
    window.setContentWrapperMarginBottom(0);
    window.setTitleBarHeight(40);
    window.resize(593,572);
    window.show();

    return a.exec();
}

