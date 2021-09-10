#include "programmer-keys-page.h"
#include "general-enum.h"
#include <QDebug>

bool ProgrammerKeysPage::m_isShifted = false;

const ProgrammerKeysPage::KeyDescription ProgrammerKeysPage::keyDescriptions[] = {
    {"Rol", Button_Key_RoL,1,0},{"Ror", Button_Key_RoR,1,1},
    {"Or", Button_Key_Or, 1, 2},{"Xor", Button_Key_Xor, 1, 3},{"Not", Button_Key_Not, 1, 4},
    {"And", Button_Key_And, 2, 0},{"Shift", Button_Key_Shift, 2, 1},{"Mod", Button_Key_Mod, 2, 2},
    {"CE", Button_Key_ClearEntry, 2, 3}, {"C", Button_Key_Clear, 2, 4},
    {"÷", Button_Key_Divide, 3, 0},{"×", Button_Key_Mult, 3, 1},{"-", Button_Key_Sub, 3, 2},
    {"+", Button_Key_Add, 3, 3},{"←", Button_Key_Backspace, 3, 4},
    {"A", Button_Key_A, 4, 0},{"B", Button_Key_B, 4, 1},{"7", Button_Key_7, 4,2},
    {"8", Button_Key_8, 4, 3},{"9", Button_Key_9, 4, 4},
    {"C", Button_Key_C, 5, 0},{"D", Button_Key_D, 5, 1},{"4", Button_Key_4, 5, 2},
    {"5", Button_Key_5, 5, 3},{"6", Button_Key_6, 5, 4},
    {"E", Button_Key_E, 6, 0},{"F", Button_Key_F, 6, 1},{"1", Button_Key_1, 6, 2},
    {"2", Button_Key_2, 6, 3},{"3", Button_Key_3, 6, 4},
    {"()", Button_Key_Brackets, 7, 0},{"±", Button_Key_Opposite, 7, 1},{"0", Button_Key_0, 7, 2},
    {".", Button_Key_Point, 7,3},{"=", Button_Key_Equal, 7, 4},

    {"Lsh", Button_Key_Lsh, 1, 0},{"Rsh", Button_Key_Rsh, 1, 1},
};

ProgrammerKeysPage::ProgrammerKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(3);
    m_gridLayout->setSpacing(3);
    initProgrammerButtons();
    switchProgrammerFormatKeys(NumFormat::Num_Format_Dec);   //默认十进制键盘

}

#define PROPERTY_KEY_ENUM "Programmer_Button_Enum"

void ProgrammerKeysPage::initProgrammerButtons()
{
    int cout = sizeof(keyDescriptions) / sizeof(keyDescriptions[0]);
    for(int i = 0; i< cout; i++)
    {
        const  KeyDescription* description = keyDescriptions + i;
        QPushButton* key = new QPushButton(description->token);
        qDebug() << "description->token__:" + description->token;

        //利用map做映射,将枚举值和按钮指针进行映射
        m_keyEnumMap[description->button] = key;

        QVariant var = static_cast<QVariant>(description->button);

        qDebug() << "description->button";
        qDebug() << description->button;

        qDebug() << "QVariant var";
        qDebug() << var;

        key->setProperty(PROPERTY_KEY_ENUM,var);
        key->setFocusPolicy(Qt::NoFocus);
        m_gridLayout->addWidget(key, description->row, description->column);
        connect(key,SIGNAL(clicked()),this,SLOT(handleProgrammerButtonClicked()));
    }
}

void ProgrammerKeysPage::handleProgrammerButtonClicked()
{
    auto keyEnumVar = sender()->property(PROPERTY_KEY_ENUM);
    int iKeyEnum = keyEnumVar.toInt();

    qDebug() << "iKeyEnum____:";
    qDebug() << iKeyEnum;

    emitProgrammerButtonPressed(iKeyEnum);
}

void ProgrammerKeysPage::switchProgrammerFormatKeys(int currentFormat)
{
    QMap<Button,QPushButton*>::iterator i;
    //按钮复位
    for(i = m_keyEnumMap.find(Button_Key_0); i != m_keyEnumMap.find((Button)16); i++)
    {
        QPushButton *key = i.value();
        key->setEnabled(true);
    }
    switch (currentFormat) {
    case Num_Format_Hex:
        break;
    case Num_Format_Dec:
        for(i = m_keyEnumMap.find(Button_Key_A); i != m_keyEnumMap.find((Button) 16);i++)                   // Button_Key_F -> 15
        {
            QPushButton *key =  i.value();
            key->setEnabled(false);
        }
        break;
    case Num_Format_Oct:
        for(i = m_keyEnumMap.find(Button_Key_8); i != m_keyEnumMap.find((Button) 16);i++)
        {
            QPushButton *key =  i.value();
            key->setEnabled(false);
        }
        break;
    case Num_Format_Bin:
        for(i = m_keyEnumMap.find(Button_Key_2); i != m_keyEnumMap.find((Button) 16);i++)
        {
            QPushButton *key =  i.value();
            key->setEnabled(false);
        }
        break;
    }
}

void ProgrammerKeysPage::switchProgrammerLogicalAndShift()
{
    QPushButton* keyLsh = m_keyEnumMap.value(Button_Key_Lsh);
    QPushButton* keyRsh = m_keyEnumMap.value(Button_Key_Rsh);
    if(!m_isShifted)
    {
        keyLsh->setVisible(false);
        keyRsh->setVisible(false);
        m_isShifted = true;
        qDebug() << "switchLogicalAndShift___m_isShifted = true";
    }
    else
    {
        keyLsh->setVisible(true);
        keyRsh->setVisible(true);
        m_isShifted = false;
        qDebug() << "switchLogicalAndShift___m_isShifted = false";
    }
}

void ProgrammerKeysPage::emitProgrammerButtonPressed(int button) const
{
    emit programmerButtonPressed(Button(button));
}
