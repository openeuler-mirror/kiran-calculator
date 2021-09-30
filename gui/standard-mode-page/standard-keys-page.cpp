#include "standard-keys-page.h"
#include <QDebug>
#include <QGridLayout>

const StandardKeysPage::KeyDescription StandardKeysPage::keyDescriptions[] = {
    {"%", Button_Key_Percent, 1, 0,"btn_percent"},     {"√", Button_Key_Sqrt, 1, 3,"btn_sqrt"},
    {"x²", Button_Key_Square, 1, 6,"btn_square"},     {"1/x", Button_Key_Reciprocal, 1, 9,"btn_reciprocal"},

    {"C", Button_Key_Clear, 2, 0,"btn_clear"},       {"CE", Button_Key_ClearEntry, 2, 3,"btn_clearEntry"},

    {"←", Button_Key_Backspace, 2, 6,"btn_backspace"},  {"÷", Button_Key_Divide, 2, 9,"btn_divide"},

    {"7", Button_Key_7, 3, 0,"btn_7"},         {"8", Button_Key_8, 3, 3,"btn_8"},
    {"9", Button_Key_9, 3, 6,"btn_9"},         {"×", Button_Key_Mult, 3, 9,"btn_mult"},

    {"4", Button_Key_4, 4, 0,"btn_4"},         {"5", Button_Key_5, 4, 3,"btn_5"},
    {"6", Button_Key_6, 4, 6,"btn_6"},         {"-", Button_Key_Sub, 4, 9,"btn_sub"},

    {"1", Button_Key_1, 5, 0,"btn_1"},         {"2", Button_Key_2, 5, 3,"btn_2"},
    {"3", Button_Key_3, 5, 6,"btn_3"},         {"+", Button_Key_Add, 5, 9,"btn_add"},

    {"()", Button_Key_Brackets, 6, 0,"btn_brackets"},  {"0", Button_Key_0, 6, 3,"btn_0"},
    {".", Button_Key_Point, 6, 6,"btn_point"}, {"=", Button_Key_Equal, 6, 9,"btn_equal"}
};


StandardKeysPage::StandardKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(4);
    m_gridLayout->setMargin(0);
    initButtons();   
}

void StandardKeysPage::emitButtonPressed(int button) const
{
    emit buttonPressed(Button(button));
}

//#include <QPainter>
//void StandardKeysPage::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.fillRect(this->rect(),QColor("red"));
//}

#define PROPERTY_KEY_ENUM "Button_Key"

void StandardKeysPage::initButtons()
{
    const int count = sizeof(keyDescriptions) / sizeof (keyDescriptions[0]);
    for(int i = 0; i < count; ++i)
    {
        const KeyDescription* description = keyDescriptions + i;
        QPushButton* key = new QPushButton (description->token,this);
        key->setObjectName(description->objectName);
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

    emitButtonPressed(iKeyEnum);
}
