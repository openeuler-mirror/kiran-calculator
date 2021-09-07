#ifndef PROGRAMMERMODEPAGE_H
#define PROGRAMMERMODEPAGE_H

#include <QWidget>
#include <QScrollBar>
#include "keys-page.h"
class Session;
class HistoryRecoder;
class StagePage;
class ProgrammerExprCalculator;
class NumConversion;
enum Button;
namespace Ui {
class ProgrammerModePage;
}

class ProgrammerModePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProgrammerModePage(QWidget *parent = nullptr);
    ~ProgrammerModePage();

public slots:
    void handleProgrammerKeysButtonPress(Button);
protected:
    virtual void showEvent(QShowEvent *event) override;
private:
    Ui::ProgrammerModePage *ui;
    Session *m_programmerSession = nullptr;
};

#endif // PROGRAMMERMODEPAGE_H
