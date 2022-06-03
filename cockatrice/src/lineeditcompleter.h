#ifndef LINEEDITCOMPLETER_H
#define LINEEDITCOMPLETER_H

#include "customlineedit.h"
#include "cardcompleter.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QStringListModel>
#include <QListView>

class TestEater : public QObject
{
    Q_OBJECT
    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;
    public:
        TestEater(QObject *parent = nullptr);
};

class LineEditCompleter : public LineEditUnfocusable
{
    Q_OBJECT
private:
    QString cursorWord(const QString &line) const;
    QCompleter *c;
    CardNameCompleter *cardCompleter;

    int lastDoubleBracketIndex;
    void moveCompleter(QCompleter *, int);

private slots:
    void insertCompletion(QString);
    void insertCardCompletion(QString);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *event);
    void focusOutEvent(QFocusEvent *e);

public:
    explicit LineEditCompleter(QWidget *parent = nullptr);
    void setCompleter(QCompleter *);
    void setCompletionList(QStringList);
};

#endif
