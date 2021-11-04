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
#include "standard-keys-page.h"
#include "animation-click-button.h"
#include <QDebug>
#include <QGridLayout>
#include <QTimer>
#include <QTest>

#define PROPERTY_KEY_ENUM "Button_Key"
const StandardKeysPage::KeyDescription StandardKeysPage::keyDescriptions[] = {
    {"%", Button_Key_Percent, 1, 0,"btn_percent"},     {"√", Button_Key_Sqrt, 1, 3,"btn_sqrt"},
    {"x²", Button_Key_Square, 1, 6,"btn_square"},     {"1/x", Button_Key_Reciprocal, 1, 9,"btn_reciprocal"},

    {"C", Button_Key_Clear, 2, 0,"btn_clear"},       {"CE", Button_Key_ClearEntry, 2, 3,"btn_clearEntry"},

    {"←", Button_Key_Backspace, 2, 6,"btn_backspace"},  {"÷", Button_Key_Divide, 2, 9,"btn_divide"},

    {"7", Button_Key_7, 3, 0,"btn_7"},         {"8", Button_Key_8, 3, 3,"btn_8"},
    {"9", Button_Key_9, 3, 6,"btn_9"},         {"×", Button_Key_Mult, 3, 9,"btn_mult"},

    {"4", Button_Key_4, 4, 0,"btn_4"},         {"5", Button_Key_5, 4, 3,"btn_5"},
    {"6", Button_Key_6, 4, 6,"btn_6"},         {"－", Button_Key_Sub, 4, 9,"btn_sub"},

    {"1", Button_Key_1, 5, 0,"btn_1"},         {"2", Button_Key_2, 5, 3,"btn_2"},
    {"3", Button_Key_3, 5, 6,"btn_3"},         {"＋", Button_Key_Add, 5, 9,"btn_add"},

    {"( )", Button_Key_Brackets, 6, 0,"btn_brackets"},  {"0", Button_Key_0, 6, 3,"btn_0"},
    {".", Button_Key_Point, 6, 6,"btn_point"}, {"＝", Button_Key_Equal, 6, 9,"btn_equal"}
};

StandardKeysPage::StandardKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(4);
    m_gridLayout->setMargin(0);
    initButtons();
}

void StandardKeysPage::initButtons()
{
    const int count = sizeof(keyDescriptions) / sizeof (keyDescriptions[0]);
    for(int i = 0; i < count; ++i)
    {
        const KeyDescription* description = keyDescriptions + i;

        auto* key = new AnimationClickButton (description->token,this);

        key->setObjectName(description->objectName);
        m_keyEnumMap[description->button] = key;
        m_gridLayout->addWidget(key,description->row,description->column);
        key->setFocusPolicy(Qt::NoFocus);

        QVariant var = static_cast<QVariant>(description->button);

        key->setProperty(PROPERTY_KEY_ENUM , var);
        connect(key,&QPushButton::clicked,this,&StandardKeysPage::handleButtonClicked);
    }
}

void StandardKeysPage::handleButtonClicked()
{

    auto keyEnumVar = sender()->property(PROPERTY_KEY_ENUM);
    int iKeyEnum = keyEnumVar.toInt();

    qDebug() << "handleButtonClicked:";
    qDebug() << iKeyEnum;

     emit buttonClicked(Button(iKeyEnum));
}

void StandardKeysPage::handleButtonAnimate(Button button)
{
    m_keyEnumMap[button]->animation();
}


