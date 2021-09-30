#ifndef STANDARDMODEPAGE_H
#define STANDARDMODEPAGE_H

#include "keys-page.h"
#include <QWidget>

class Session;
class StandardKeysPage;
class ExprCalculator;
class HistoryRecoder;
class StagePage;
enum Button;
namespace Ui {
class StandardModePage;
}

class StandardModePage : public QWidget
{
    Q_OBJECT

public:
    explicit StandardModePage(QWidget *parent = nullptr);
    ~StandardModePage();


public slots:
    void handleStandardKeysButtonPress(Button);


protected:
    virtual void showEvent(QShowEvent *event) override;
private:
    Ui::StandardModePage *ui;
    Session *m_standardSession = nullptr;
};

#endif // STANDARDMODEPAGE_H
