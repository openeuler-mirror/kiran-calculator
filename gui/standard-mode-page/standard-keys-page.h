#ifndef STANDARDKEYSPAGE_H
#define STANDARDKEYSPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QObject>
#include "keys-page.h"

enum Button;
class QPushButton;
class QGridLayout;

class StandardKeysPage : public QWidget
{
    Q_OBJECT
public:
    explicit StandardKeysPage(QWidget *parent = nullptr);

    void initButtons();

public slots:
    void handleButtonClicked();

protected slots:
    void emitButtonPressed(int button) const;
//    void paintEvent(QPaintEvent *event) override;

private:
    static const struct KeyDescription{
        QString token;
        Button button;
        int row;
        int column;
    }keyDescriptions[];

    QGridLayout* m_gridLayout = nullptr;

signals:
    void buttonPressed(Button) const;

};

#endif // STANDARDKEYSPAGE_H
