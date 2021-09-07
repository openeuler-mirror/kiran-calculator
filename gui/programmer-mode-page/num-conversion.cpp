#include "num-conversion.h"
#include "core/sessionhistory.h"
#include "core/session.h"
#include "core/numberformatter.h"
#include "core/evaluator.h"
#include "utils.h"
#include <QDebug>

NumConversion::NumConversion(QWidget* parent) : QTableWidget(parent)
{
    connect(this, SIGNAL(itemClicked(QTableWidgetItem*)),this,
            SLOT(activateNumConversion(QTableWidgetItem*)));

    m_evaluator = Evaluator::instance();

//    setCurrentItem(item(1,0));
}

void NumConversion::showNumFormatConverted(const Quantity & quantity)
{
    //重载运算符'+'  使得进制和模式可以组合
    QString formatBin = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Binary() + Quantity::Format::Fixed()).remove("0b");
    QString formatOct = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Octal() + Quantity::Format::Fixed()).remove("0o");
    QString formatHex = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Hexadecimal() + Quantity::Format::Fixed()).remove("0x");
    QString formatDec = DMath::format(quantity, Quantity::Format::Complement() + Quantity::Format::Decimal() + Quantity::Format::Fixed());
    //    QString formatDec = NumberFormatter::format(quantity);

    qDebug() << "NumConversion_formatDec:" + formatDec;

    //二进制补0
    int i = formatBin.length() % 4;
    if (formatBin != "0") {
        while (i != 4 && i != 0) {
            formatBin.insert(0, "0");
            i++;
        }
    }

    formatHex = Utils::reformatSeparatorsPro(formatHex, 16);
    formatOct = Utils::reformatSeparatorsPro(formatOct, 8);
    formatBin = Utils::reformatSeparatorsPro(formatBin, 2);
    formatDec = Utils::reformatSeparatorsPro(formatDec, 10);


    item(0,0)->setText(formatHex);
    item(1,0)->setText(formatDec);
    item(2,0)->setText(formatOct);
    item(3,0)->setText(formatBin);
    qDebug() << "showNumFormatConverted";
}

void NumConversion::activateNumConversion(QTableWidgetItem* item)
{
    qDebug() << "row:";
    qDebug() << item->row();

    emit numConvered(item->row());
    emit refreshNumFormatHistory( );
    emit refreshNumFormatStage();
}


void NumConversion::clearItems()
{
    qDebug() << "clearItems";
    item(0,0)->setText("");
    item(1,0)->setText("");
    item(2,0)->setText("");
    item(3,0)->setText("");
}
