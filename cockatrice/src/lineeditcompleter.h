#ifndef LINEEDITCOMPLETER_H
#define LINEEDITCOMPLETER_H

#include "customlineedit.h"
#include "cardcompleter.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QStringListModel>
#include <QListView>

class LineEditCompleter : public LineEditUnfocusable
{
    Q_OBJECT
private:
    QString cursorWord(const QString &line) const;
    QCompleter *c;
    CardNameCompleter *cardCompleter;

    int lastDoubleBracketIndex;
    void moveCompleter(int);

private slots:
    void insertCompletion(QString);
    void insertCardCompletion(QString);

protected:
    void keyPressEvent(QKeyEvent *event);
    bool event(QEvent *event);

public:
    explicit LineEditCompleter(QWidget *parent = nullptr);
    void setCompleter(QCompleter *);
    void setCompletionList(QStringList);
};

#endif
