#include "gui/window.h"
#include <QDebug>
#include <QApplication>

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
    Window w;
    w.show();
    return a.exec();
}
