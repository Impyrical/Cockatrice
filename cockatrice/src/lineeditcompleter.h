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
    bool inCardComplete;

    QStringListModel *cardModel;
    QListView *cardCompleteList;

private slots:
    void insertCompletion(QString);
    void toggleCardCompletion(const QStringList &);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusOutEvent(QFocusEvent *e);

public:
    explicit LineEditCompleter(QWidget *parent = nullptr);
    void setCompleter(QCompleter *);
    void setCompletionList(QStringList);
    void setCardCompleter(CardNameCompleter *);
};

#endif
