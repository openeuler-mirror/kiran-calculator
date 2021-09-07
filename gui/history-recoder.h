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

    enum NumFormat{
        Num_Format_Hex,Num_Format_Dec,Num_Format_Oct,Num_Format_Bin
    };

    static int m_currentFormat;
    static int m_historyRecordPoint;

public slots:
    void updateHistory();

    void NumFormateHistory();
    void historyFormatChanged(int );
    void clearHistory();
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
