#ifndef HISTORYRECODER_H
#define HISTORYRECODER_H

#include <QWidget>
#include <QListWidget>

class QListWidget;
class QListWidgetItem;
class HistoryEntry;
class Evaluator;
class Session;
class ProgrammerExprCalculator;

class HistoryRecoder : public QListWidget
{
    Q_OBJECT
public:
    explicit HistoryRecoder(QWidget *parent = nullptr);
    void setSession(Session*);

    static int m_currentFormat;
    static bool m_FE;

public slots:
    void updateStandardHistory();
    void updateScienceHistory();
    void updateProgrammerHistory();

    void historyFEChanged();
    void historyFormatChanged(int );
    void clearStandardHistory();
    void clearScienceHistory();
    void clearProgrammerHistory();

protected slots:
    void handleItem(QListWidgetItem *);

private:
    Session* m_session;
    Evaluator* m_evaluator;
    ProgrammerExprCalculator* m_programmerExpr;

    QVector<QString> m_numvec;
    QVector<QString> m_opvec;
    QString m_textorder;


signals:
    void exprSelected(const QString &);

};

#endif // HISTORYRECODER_H
