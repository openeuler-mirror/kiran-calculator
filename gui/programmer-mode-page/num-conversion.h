#ifndef NUMCONVERSION_H
#define NUMCONVERSION_H
#include <QWidget>
#include <QTableWidget>
#include "core/session.h"
#include "core/sessionhistory.h"
#include <QString>
class Evaluator;
class Session;
class NumConversion : public QTableWidget
{
    Q_OBJECT
public:
    NumConversion(QWidget* parent = nullptr);


public slots:
    void showNumFormatConverted(const Quantity&);
    void clearItems();
    void activateNumConversion(QTableWidgetItem*);


private:
    Evaluator* m_evaluator;
    Session* m_programmerSession;
    QString m_messageDec;

signals:
    void resultSelected(const QString&);
    void numConvered(int );
    void refreshNumFormatHistory( );
    void refreshNumFormatStage();
};

#endif // NUMCONVERSION_H
