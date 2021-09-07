#ifndef SCIENCEKEYSPAGE_H
#define SCIENCEKEYSPAGE_H
#include "keys-page.h"
#include <QWidget>
#include <QPushButton>
#include <QVariant>
#include <QObject>
#include <QGridLayout>

enum Button;
class QPushButton;

class ScienceKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit ScienceKeysPage(QWidget *parent = nullptr);
    void initScienceButtons();

    static int m_angleUnit;
    enum AngleUnit{
        Angle_Unit_Degree,Angle_Unit_Radian,Angle_Unit_Gradian,
    };
public slots:
    void handleScienceButtonClicked();
    void switchScienceAngleUnit();

protected slots:
    void emitScienceButtonPressed(int button) const;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
    }keyDescriptions[];

    QGridLayout* m_gridLayout = nullptr;
    QMap<Button, QPushButton*> m_keyEnumMap;

signals:
    void scienceButtonPressed(Button) const;
    void scienceSwitchAngleUnit(int );
};

#endif // SCIENCEKEYSPAGE_H
