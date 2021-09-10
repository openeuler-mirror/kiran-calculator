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
    static bool m_isShift;
    static bool m_isHYP;

public slots:
    void handleScienceButtonClicked();
    void switchScienceAngleUnit();
    void switchScienceHYP();
    void shiftScienceFunction();

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

    QPushButton* keySin;
    QPushButton* keyCos;
    QPushButton* keyTan;

signals:
    void scienceButtonPressed(Button) const;
    void scienceSwitchAngleUnit(int );
    void scienceIsShift();
    void scienceIsHYP();
};

#endif // SCIENCEKEYSPAGE_H
