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
#include "programmer-keys-page.h"
#include "general-enum.h"
#include <QDebug>

bool ProgrammerKeysPage::m_isShifted = false;

const ProgrammerKeysPage::KeyDescription ProgrammerKeysPage::keyDescriptions[] = {
    {"Lsh", Button_Key_Lsh, 1, 0,"btn_lsh"},{"Rsh", Button_Key_Rsh, 1, 1,"btn_rsh"},
    {"Or", Button_Key_Or, 1, 2,"btn_or"},{"Xor", Button_Key_Xor, 1, 3,"btn_xor"},{"Not", Button_Key_Not, 1, 4,"btn_not"},
    {"And", Button_Key_And, 2, 0,"btn_and"},{"↑", Button_Key_Shift, 2, 1,"btn_shift"},{"Mod", Button_Key_Mod, 2, 2,"btn_mod"},
    {"CE", Button_Key_ClearEntry, 2, 3,"btn_clearEntry"}, {"C", Button_Key_Clear, 2, 4,"btn_clear"},
    {"÷", Button_Key_Divide, 3, 0,"btn_divide"},{"×", Button_Key_Mult, 3, 1,"btn_mult"},{"－", Button_Key_Sub, 3, 2,"btn_sub"},
    {"＋", Button_Key_Add, 3, 3,"btn_add"},{"←", Button_Key_Backspace, 3, 4,"btn_backspace"},
    {"A", Button_Key_A, 4, 0,"btn_a"},{"B", Button_Key_B, 4, 1,"btn_b"},{"7", Button_Key_7, 4,2,"btn_7"},
    {"8", Button_Key_8, 4, 3,"btn_8"},{"9", Button_Key_9, 4, 4,"btn_9"},
    {"C", Button_Key_C, 5, 0,"btn_c"},{"D", Button_Key_D, 5, 1,"btn_d"},{"4", Button_Key_4, 5, 2,"btn_4"},
    {"5", Button_Key_5, 5, 3,"btn_5"},{"6", Button_Key_6, 5, 4,"btn_6"},
    {"E", Button_Key_E, 6, 0,"btn_e"},{"F", Button_Key_F, 6, 1,"btn_f"},{"1", Button_Key_1, 6, 2,"btn_1"},
    {"2", Button_Key_2, 6, 3,"btn_2"},{"3", Button_Key_3, 6, 4,"btn_3"},
    {"(", Button_Key_LeftBracket, 7, 0,"btn_leftBracket"}, {")", Button_Key_RightBracket, 7, 1,"btn_rightBracket"},
//    {"±", Button_Key_Opposite, 7, 1},   //暂时禁用取反函数
    {"0", Button_Key_0, 7, 2,"btn_0"},
    {".", Button_Key_Point, 7,3,"btn_point"},{"＝", Button_Key_Equal, 7, 4,"btn_equal"},

};

ProgrammerKeysPage::ProgrammerKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(3);
    m_gridLayout->setMargin(0);
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

        key->setObjectName(description->objectName);

        //程序员模式下，禁用小数点
        if(description->button == Button_Key_Point)
            key->setEnabled(false);
        if(description->button == Button_Key_Shift)
            key->setCheckable(true);

        //利用map做映射,将枚举值和按钮指针进行映射
        m_keyEnumMap[description->button] = key;

        QVariant var = static_cast<QVariant>(description->button);

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
    default:
        for(i = m_keyEnumMap.find(Button_Key_A); i != m_keyEnumMap.find((Button) 16);i++)                   // Button_Key_F -> 15
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
        keyLsh->setText("Rol");
        keyRsh->setText("Ror");
        m_isShifted = true;
        qDebug() << "switchLogicalAndShift___m_isShifted = true";
        emit programmerIsShift();

    }
    else
    {
        keyLsh->setText("Lsh");
        keyRsh->setText("Rsh");
        m_isShifted = false;
        qDebug() << "switchLogicalAndShift___m_isShifted = false";
        emit programmerIsShift();

    }
}

void ProgrammerKeysPage::emitProgrammerButtonPressed(int button) const
{
    emit programmerButtonPressed(Button(button));
}
