#include "standard-keys-page.h"
#include <QDebug>
#include <QGridLayout>

const StandardKeysPage::KeyDescription StandardKeysPage::keyDescriptions[] = {
    {"%", Button_Key_Percent, 1, 0},     {"√", Button_Key_Sqrt, 1, 3},
    {"x²", Button_Key_Square, 1, 6},     {"1/x", Button_Key_Reciprocal, 1, 9},

    {"C", Button_Key_Clear, 2, 0},       {"CE", Button_Key_ClearEntry, 2, 3},{"()", Button_Key_Brackets, 2, 6},

    {"←", Button_Key_Backspace, 2, 9},  {"÷", Button_Key_Divide, 2, 12},

    {"7", Button_Key_7, 3, 0},         {"8", Button_Key_8, 3, 3},
    {"9", Button_Key_9, 3, 6},         {"×", Button_Key_Mult, 3, 9},

    {"4", Button_Key_4, 4, 0},         {"5", Button_Key_5, 4, 3},
    {"6", Button_Key_6, 4, 6},         {"-", Button_Key_Sub, 4, 9},

    {"1", Button_Key_1, 5, 0},         {"2", Button_Key_2, 5, 3},
    {"3", Button_Key_3, 5, 6},         {"+", Button_Key_Add, 5, 9},

    {"±", Button_Key_Opposite, 6, 0},  {"0", Button_Key_0, 6, 3},
    {".", Button_Key_Point, 6, 6}, {"=", Button_Key_Equal, 6, 9}
};


StandardKeysPage::StandardKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(3);
    m_gridLayout->setSpacing(3);
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
        QPushButton* key = new QPushButton (description->token);

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
