#ifndef SCIENCEMODEPAGE_H
#define SCIENCEMODEPAGE_H

#include <QWidget>
#include "keys-page.h"
class Session;
class ScienceKeysPage;
class HistoryRecoder;
class StagePage;
class ScienceExprCalculator;
enum Button;
namespace Ui {
class ScienceModePage;
}

class ScienceModePage : public QWidget
{
    Q_OBJECT

public:
    explicit ScienceModePage(QWidget *parent = nullptr);
    ~ScienceModePage();

protected:
    virtual void showEvent(QShowEvent *event) override;
private:
    Ui::ScienceModePage *ui;
    Session *m_scienceSession = nullptr;

public slots:
    void handleScienceKeysButtonPress(Button);
};

#endif // SCIENCEMODEPAGE_H
