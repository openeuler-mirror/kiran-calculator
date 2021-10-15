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
#include "science-keys-page.h"
#include "general-enum.h"

int ScienceKeysPage::m_angleUnit = 0;
bool ScienceKeysPage::m_isShift = false;
bool ScienceKeysPage::m_isHYP = false;

const ScienceKeysPage::KeyDescription ScienceKeysPage::keyDescriptions[] = {
//    {"10ˣ", Button_Key_10n, 6, 1},

    {"DEG", Button_Key_AngleUnit,0, 0,"btn_DEG",1,1},{"HYP", Button_Key_HYP, 0, 1,"btn_HYP",1,1},{"F-E", Button_Key_FE,0,2,"btn_FE",1,1},

//    {"x^3", Button_Key_Cube, 1, 0},{"x^-y", Button_Key_Yroot, 1, 1},{"arcsin", Button_Key_Arcsin, 1, 2},
//    {"acrcos", Button_Key_Arccos, 1, 3}, {"arctan", Button_Key_Arctan, 1, 4},
//    {"1/x", Button_Key_Reciprocal, 2, 0}, {"e^x", Button_Key_Index, 2, 1}, {"ln", Button_Key_Ln, 2, 2},

    {"x²", Button_Key_Square, 1, 0,"btn_square",1,1},{"x^n", Button_Key_Xn, 1, 1,"btn_xn",1,1},{"sin", Button_Key_Sin, 1, 2,"btn_sin",1,1},
    {"cos", Button_Key_Cos, 1, 3,"btn_cos",1,1}, {"tan", Button_Key_Tan, 1, 4,"btn_tan",1,1},
    {"√", Button_Key_Sqrt, 2, 0,"btn_sqrt",1,1}, {"10^n", Button_Key_10n, 2, 1,"btn_10n",1,1}, {"log", Button_Key_Log, 2, 2,"btn_log",1,1},

    {"Exp", Button_Key_Exp, 2, 3,"btn_exp",1,1}, {"Mod", Button_Key_Mod, 2, 4,"btn_mod",1,1},
    {"shift", Button_Key_Shift, 3, 0,"btn_shift",1,1},{"CE", Button_Key_ClearEntry, 3, 1,"btn_clearEntry",1,1},{"C", Button_Key_Clear, 3, 2,"btn_clear",1,1},
    {"←", Button_Key_Backspace, 3, 3,"btn_backspace",1,1},{"÷", Button_Key_Divide, 3, 4,"btn_divide",1,1},
    {"π", Button_Key_Pi, 4, 0,"btn_pi",1,1},    {"7", Button_Key_7, 4, 1,"btn_7",1,1},   {"8", Button_Key_8, 4, 2,"btn_8",1,1},
    {"9", Button_Key_9, 4, 3,"btn_9",1,1}, {"×", Button_Key_Mult, 4, 4,"btn_mult",1,1},
    {"n!", Button_Key_Factorials, 5, 0,"btn_factorials",1,1}, {"4", Button_Key_4, 5, 1,"btn_4",1,1}, {"5", Button_Key_5, 5, 2,"btn_5",1,1}, {"6", Button_Key_6, 5, 3,"btn_6",1,1},
    {"-", Button_Key_Sub, 5, 4,"btn_sub",1,1},
    //暂时禁止取反函数
//    {"±", Button_Key_Opposite, 6, 0,},
    {"(", Button_Key_LeftBracket, 6, 0,"btn_leftBracket",1,1},
    {"1", Button_Key_1, 6, 1,"btn_1",1,1},    {"2", Button_Key_2, 6, 2,"btn_2",1,1},  {"3", Button_Key_3, 6, 3,"btn_3",1,1},
    {"+", Button_Key_Add, 6, 4,"btn_add",1,1},
    {")", Button_Key_RightBracket, 7, 0,"btn_rightBracket",1,1}, {"0", Button_Key_0, 7, 1,"btn_0",1,1}, {".", Button_Key_Point, 7, 2,"btn_point",1,1},{"=", Button_Key_Equal, 7, 3,"btn_equal",1,2},

};

ScienceKeysPage::ScienceKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(3);
    m_gridLayout->setMargin(0);
    initScienceButtons();
}

void ScienceKeysPage::emitScienceButtonPressed(int button) const
{
    emit scienceButtonPressed(Button(button));
}

#define PROPERTY_KEY_ENUM "Science_Button_Enum"
void ScienceKeysPage::initScienceButtons()
{
    int cout  = sizeof(keyDescriptions) / sizeof (keyDescriptions[0]);
    for(int i = 0; i < cout; ++i)
    {
        const KeyDescription* description = keyDescriptions + i;
        QPushButton* key = new QPushButton(description->token);
        key->setObjectName(description->objectName);
        m_keyEnumMap[description->button] = key;

        if((description->button == Button_Key_Shift)
                || (description->button == Button_Key_HYP)
                || (description->button == Button_Key_FE))
            key->setCheckable(true);

        QVariant var = static_cast<QVariant>(description->button);

        key->setProperty(PROPERTY_KEY_ENUM,var);
        key->setFocusPolicy(Qt::NoFocus);
        m_gridLayout->addWidget(key, description->row, description->column,description->rowcount,description->columncount);

        connect(key,SIGNAL(clicked()),this,SLOT(handleScienceButtonClicked()));
    }
    keySin = m_keyEnumMap.value(Button_Key_Sin);
    keyCos = m_keyEnumMap.value(Button_Key_Cos);
    keyTan = m_keyEnumMap.value(Button_Key_Tan);
}

void  ScienceKeysPage::handleScienceButtonClicked()
{
    auto keyEnumVar = sender()->property(PROPERTY_KEY_ENUM);
    int iKeyEnum = keyEnumVar.toInt();

    emitScienceButtonPressed(iKeyEnum);
}

void ScienceKeysPage::switchScienceAngleUnit()
{
    QPushButton* keyAngleUnit = m_keyEnumMap.value(Button_Key_AngleUnit);

    if (m_angleUnit == Angle_Unit_Degree) {
        m_angleUnit = Angle_Unit_Radian;
        keyAngleUnit->setText("RAD");
    } else if (m_angleUnit == Angle_Unit_Radian) {
        m_angleUnit = Angle_Unit_Gradian;
        keyAngleUnit->setText("GRAD");
    } else if (m_angleUnit == Angle_Unit_Gradian) {
        m_angleUnit = Angle_Unit_Degree;
        keyAngleUnit->setText("DEG");
    }

    emit scienceSwitchAngleUnit(m_angleUnit);
}

void ScienceKeysPage::switchScienceHYP()
{
    if(!m_isHYP)
    {
        if(m_isShift)
        {
            keySin->setText("arsinh");
            keyCos->setText("arcosh");
            keyTan->setText("artanh");
            m_isHYP = true;
            emit scienceIsHYP();
        }
        else
        {
            keySin->setText("sinh");
            keyCos->setText("cosh");
            keyTan->setText("tanh");
            m_isHYP = true;
            emit scienceIsHYP();
        }
    }
    else
    {
        if(m_isShift)
        {
            keySin->setText("arcsin");
            keyCos->setText("arccos");
            keyTan->setText("arctan");
            m_isHYP = false;
            emit scienceIsHYP();
        }
        else
        {
            keySin->setText("sin");
            keyCos->setText("cos");
            keyTan->setText("tan");
            m_isHYP = false;
            emit scienceIsHYP();
        }
    }
}

void ScienceKeysPage::shiftScienceFunction()
{
    QPushButton* key_shift = m_keyEnumMap.value(Button_Key_Shift);   //待增加切换动画


    QPushButton* keySquare = m_keyEnumMap.value(Button_Key_Square);
    QPushButton* keyXn = m_keyEnumMap.value(Button_Key_Xn);
    QPushButton* keySqrt = m_keyEnumMap.value(Button_Key_Sqrt);
    QPushButton* key10n = m_keyEnumMap.value(Button_Key_10n);
    QPushButton* keyLog = m_keyEnumMap.value(Button_Key_Log);

    if(!m_isShift)
    {
        keySquare->setText("x^3");
        keyXn->setText("x^-y");
        keySqrt->setText("1/x");
        key10n->setText("e^x");
        keyLog->setText("ln");

        if(m_isHYP)
        {
            keySin->setText("arsinh");
            keyCos->setText("arcosh");
            keyTan->setText("artanh");
        }
        else
        {
            keySin->setText("arcsin");
            keyCos->setText("arccos");
            keyTan->setText("arctan");
        }
        m_isShift = true;
        emit scienceIsShift();
    }
    else
    {
        keySquare->setText("x²");
        keyXn->setText("x^n");
        keySqrt->setText("√");
        key10n->setText("10^n");
        keyLog->setText("log");
        if(m_isHYP)
        {
            keySin->setText("sinh");
            keyCos->setText("cosh");
            keyTan->setText("tanh");
        }
        else
        {
            keySin->setText("sin");
            keyCos->setText("cos");
            keyTan->setText("tan");
        }
        m_isShift = false;
        emit scienceIsShift();
    }
}

