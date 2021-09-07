/********************************************************************************
** Form generated from reading UI file 'mode-switcher-page.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODE_2D_SWITCHER_2D_PAGE_H
#define UI_MODE_2D_SWITCHER_2D_PAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModeSwitcherPage
{
public:
    QListWidget *modeListWidget;

    void setupUi(QWidget *ModeSwitcherPage)
    {
        if (ModeSwitcherPage->objectName().isEmpty())
            ModeSwitcherPage->setObjectName(QStringLiteral("ModeSwitcherPage"));
        ModeSwitcherPage->resize(446, 356);
        ModeSwitcherPage->setStyleSheet(QStringLiteral("QWidget{border:1px solid red;}"));
        modeListWidget = new QListWidget(ModeSwitcherPage);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(modeListWidget);
        __qlistwidgetitem->setTextAlignment(Qt::AlignCenter);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(modeListWidget);
        __qlistwidgetitem1->setTextAlignment(Qt::AlignCenter);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(modeListWidget);
        __qlistwidgetitem2->setTextAlignment(Qt::AlignCenter);
        QListWidgetItem *__qlistwidgetitem3 = new QListWidgetItem(modeListWidget);
        __qlistwidgetitem3->setTextAlignment(Qt::AlignCenter);
        modeListWidget->setObjectName(QStringLiteral("modeListWidget"));
        modeListWidget->setGeometry(QRect(9, 9, 91, 338));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(modeListWidget->sizePolicy().hasHeightForWidth());
        modeListWidget->setSizePolicy(sizePolicy);

        retranslateUi(ModeSwitcherPage);

        QMetaObject::connectSlotsByName(ModeSwitcherPage);
    } // setupUi

    void retranslateUi(QWidget *ModeSwitcherPage)
    {
        ModeSwitcherPage->setWindowTitle(QApplication::translate("ModeSwitcherPage", "Form", nullptr));

        const bool __sortingEnabled = modeListWidget->isSortingEnabled();
        modeListWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = modeListWidget->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("ModeSwitcherPage", "\346\240\207\345\207\206", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = modeListWidget->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("ModeSwitcherPage", "\347\247\221\345\255\246", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = modeListWidget->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("ModeSwitcherPage", "\347\250\213\345\272\217\345\221\230", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = modeListWidget->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("ModeSwitcherPage", "\345\205\263\344\272\216", nullptr));
        modeListWidget->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class ModeSwitcherPage: public Ui_ModeSwitcherPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODE_2D_SWITCHER_2D_PAGE_H
