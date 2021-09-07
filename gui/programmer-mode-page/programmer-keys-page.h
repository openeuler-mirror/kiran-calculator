#ifndef PROGRAMMERKEYSPAGE_H
#define PROGRAMMERKEYSPAGE_H
#include "keys-page.h"
#include <QWidget>
#include <QPushButton>
#include <QVariant>
#include <QGridLayout>
#include <QMap>
enum Button;

enum NumFormat{
    Num_Format_Hex,Num_Format_Dec,Num_Format_Oct,Num_Format_Bin
};

class ProgrammerKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProgrammerKeysPage(QWidget *parent = nullptr);
    void initProgrammerButtons();
    void switchProgrammerLogicalAndShift();

    static bool m_isShifted;

public slots:
    void handleProgrammerButtonClicked();
    void switchProgrammerFormatKeys(int );

protected slots:
    void emitProgrammerButtonPressed(int button) const;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
    }keyDescriptions[];

    QGridLayout* m_gridLayout;
    QMap<Button, QPushButton*> m_keyEnumMap;
signals:
    void programmerButtonPressed(Button) const;

};
#endif // PROGRAMMERKEYSPAGE_H
