#include "gui/calculator.h"
#include <QDebug>
#include <QApplication>
#include <kiran-titlebar-window.h>

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
    QApplication a(argc, argv);
    loadStylesheet();
    KiranTitlebarWindow window;
    window.setTitle("计算器");
    window.setIcon(QIcon(":/kiran-calculator-images/图标.png"));


    window.setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    window.setResizeable(false);
    Calculator calculator;
    window.setWindowContentWidget(&calculator);
    window.setContentWrapperMarginBottom(0);
    window.setTitleBarHeight(40);
    window.resize(593,572);
    window.show();

    return a.exec();
}
