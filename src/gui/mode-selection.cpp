#include "mode-selection.h"
#include <QMenu>
#include <qt5-log-i.h>
#include "calculator.h"
#include "general-enum.h"


ModeSelection::ModeSelection(Calculator* calculator,QWidget *parent) : QPushButton(parent)
{
    setFixedSize(16,16);
    setIcon(iconColorSwitch(":/kiran-calculator-images/kc-mode.svg"));
    setIconSize(QSize(16,16));
    setFocusPolicy(Qt::NoFocus);
    m_calculator = calculator;
    initMenu();

    connect(m_standard,&QAction::triggered,m_calculator,
            [=](){
                m_calculator->switchKeyborad(Calculation_Mode_Standard);});
    connect(m_science,&QAction::triggered,m_calculator,
            [=](){
                m_calculator->switchKeyborad(Calculation_Mode_Science);});
    connect(m_programmer,&QAction::triggered,m_calculator,
            [=](){
                m_calculator->switchKeyborad(Calculation_Mode_Programmer);});
    connect(Kiran::StylePalette::instance(),&Kiran::StylePalette::themeChanged,this,&ModeSelection::handleThemeChanged);
}

void ModeSelection::initMenu()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_menu = new QMenu(this);
    m_standard = new QAction(tr("standard"));
    m_science = new QAction(tr("science"));
    m_programmer = new QAction(tr("programmer"));

    m_actionGroup = new QActionGroup(this);
    m_actionGroup->addAction(m_standard);
    m_actionGroup->addAction(m_science);
    m_actionGroup->addAction(m_programmer);

    m_standard->setCheckable(true);
    m_standard->setChecked(true);
    m_science->setCheckable(true);
    m_programmer->setCheckable(true);

    m_menu->addAction(m_standard);
    m_menu->addAction(m_science);
    m_menu->addAction(m_programmer);
    m_menu->setMinimumWidth(108);

//    setMenu(m_menu);
}

void ModeSelection::setPos(ModeSelection* button)
{
    auto buttonSzie = button->sizeHint();
    auto menuSize = m_menu->sizeHint();

    int x = button->rect().center().x() - (menuSize.width()/2);
    int y = button->rect().bottomLeft().y() + 4;

    KLOG_INFO() << "button->rect().center().x() :" << x;
    KLOG_INFO() << "button->rect().bottomLeft().y() :" << y;

    connect(this,&ModeSelection::clicked,this,
            [=](){
              m_menu->exec(button->mapToGlobal(QPoint(x,y)));}
            );
}

void ModeSelection::handleThemeChanged(Kiran::PaletteType paletteType)
{
    setIcon(iconColorSwitch(":/kiran-calculator-images/kc-mode.svg"));
}

QPixmap ModeSelection::iconColorSwitch(const QString& iconPath)
{
    QIcon icon(iconPath);
    QPixmap pixmap = icon.pixmap(16, 16);
    if (Kiran::StylePalette::instance()->paletteType() != Kiran::PALETTE_DARK)
    {
        QImage image = pixmap.toImage();
        image.invertPixels(QImage::InvertRgb);
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
}


