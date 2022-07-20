#ifndef MODESELECTION_H
#define MODESELECTION_H
#include <QPushButton>
#include <QActionGroup>
#include "style-palette.h"
class Calculator;
class ModeSelection : public QPushButton
{
    Q_OBJECT
public:
    ModeSelection(Calculator* calculator, QWidget *parent=nullptr);
    void initMenu();
    void setPos(ModeSelection* button);
    QPixmap iconColorSwitch(const QString &iconPath);

    QPoint m_menuPoint;
public slots:
    void handleThemeChanged(Kiran::PaletteType paletteType);
private:
    QMenu* m_menu;
    QAction* m_standard;
    QAction* m_science;
    QAction* m_programmer;
    QActionGroup *m_actionGroup;
    Calculator *m_calculator;
};

#endif // MODESELECTION_H
