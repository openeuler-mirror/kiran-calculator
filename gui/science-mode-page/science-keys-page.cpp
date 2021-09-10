#include "science-keys-page.h"
#include "general-enum.h"

int ScienceKeysPage::m_angleUnit = 0;
bool ScienceKeysPage::m_isShift = false;
bool ScienceKeysPage::m_isHYP = false;

const ScienceKeysPage::KeyDescription ScienceKeysPage::keyDescriptions[] = {
//    {"10ˣ", Button_Key_10n, 6, 1},

    {"DEG", Button_Key_AngleUnit,0, 0},{"HYP", Button_Key_HYP, 0, 1},{"F-E", Button_Key_FE,0,2},

//    {"x^3", Button_Key_Cube, 1, 0},{"x^-y", Button_Key_Yroot, 1, 1},{"arcsin", Button_Key_Arcsin, 1, 2},
//    {"acrcos", Button_Key_Arccos, 1, 3}, {"arctan", Button_Key_Arctan, 1, 4},
//    {"1/x", Button_Key_Reciprocal, 2, 0}, {"e^x", Button_Key_Index, 2, 1}, {"ln", Button_Key_Ln, 2, 2},

    {"x²", Button_Key_Square, 1, 0},{"x^n", Button_Key_Xn, 1, 1},{"sin", Button_Key_Sin, 1, 2},
    {"cos", Button_Key_Cos, 1, 3}, {"tan", Button_Key_Tan, 1, 4},
    {"√", Button_Key_Sqrt, 2, 0}, {"10^n", Button_Key_10n, 2, 1}, {"log", Button_Key_Log, 2, 2},

    {"Exp", Button_Key_Exp, 2, 3}, {"Mod", Button_Key_Mod, 2, 4},
    {"shift", Button_Key_Shift, 3, 0},{"CE", Button_Key_ClearEntry, 3, 1},{"C", Button_Key_Clear, 3, 2},
    {"←", Button_Key_Backspace, 3, 3},{"÷", Button_Key_Divide, 3, 4},
    {"π", Button_Key_Pi, 4, 0},    {"7", Button_Key_7, 4, 1},   {"8", Button_Key_8, 4, 2},
    {"9", Button_Key_9, 4, 3}, {"×", Button_Key_Mult, 4, 4},
    {"n!", Button_Key_Factorials, 5, 0}, {"4", Button_Key_4, 5, 1}, {"5", Button_Key_5, 5, 2}, {"6", Button_Key_6, 5, 3},
    {"-", Button_Key_Sub, 5, 4},
    {"±", Button_Key_Opposite, 6, 0},{"1", Button_Key_1, 6, 1},    {"2", Button_Key_2, 6, 2},  {"3", Button_Key_3, 6, 3},
    {"+", Button_Key_Add, 6, 4},
    {"()", Button_Key_Brackets, 7, 0}, {"0", Button_Key_0, 7, 1}, {".", Button_Key_Point, 7, 2},{"=", Button_Key_Equal, 7, 3},




};

ScienceKeysPage::ScienceKeysPage(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setMargin(3);
    m_gridLayout->setSpacing(3);
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

        m_keyEnumMap[description->button] = key;
        QVariant var = static_cast<QVariant>(description->button);

        key->setProperty(PROPERTY_KEY_ENUM,var);

        key->setFocusPolicy(Qt::NoFocus);
        m_gridLayout->addWidget(key, description->row, description->column);

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

/*----
void ScienceKeysPage::shiftScienceFunction()
{
    QPushButton* keyShift = m_keyEnumMap.value(Button_Key_Shift);   //待增加切换动画

    QPushButton* keySquare = m_keyEnumMap.value(Button_Key_Square);
    QPushButton* keyXn = m_keyEnumMap.value(Button_Key_Xn);
    QPushButton* keySin = m_keyEnumMap.value(Button_Key_Sin);
    QPushButton* keyCos = m_keyEnumMap.value(Button_Key_Cos);
    QPushButton* keyTan = m_keyEnumMap.value(Button_Key_Tan);
    QPushButton* keySqrt = m_keyEnumMap.value(Button_Key_Sqrt);
    QPushButton* key10n = m_keyEnumMap.value(Button_Key_10n);
    QPushButton* keyLog = m_keyEnumMap.value(Button_Key_Log);

    if(!m_isShift)
    {
        keySquare->setVisible(false);
        keyXn->setVisible(false);
        keySin->setVisible(false);
        keyCos->setVisible(false);
        keyTan->setVisible(false);
        keySqrt->setVisible(false);
        key10n->setVisible(false);
        keyLog->setVisible(false);
        m_isShift = true;
        emit scienceIsShift(m_isShift);
    }
    else
    {
        keySquare->setVisible(true);
        keyXn->setVisible(true);
        keySin->setVisible(true);
        keyCos->setVisible(true);
        keyTan->setVisible(true);
        keySqrt->setVisible(true);
        key10n->setVisible(true);
        keyLog->setVisible(true);
        m_isShift = false;
        emit scienceIsShift(m_isShift);
    }
}
*/
