#ifndef CARDCOMPLETER_H
#define CARDCOMPLETER_H

#include <QKeyEvent>
#include <QStringList>
#include <QCompleter>
#include <QAbstractItemView>
#include <QCompleter>

typedef QMultiHash<QString, int> TrigramIndex;

class CardNameCompleter : public QCompleter
{
    Q_OBJECT
    protected:
        TrigramIndex lookupIndex;
        QStringList cardNameList;

    private:
        void indexName(const QString *, int);

    public:
        explicit CardNameCompleter(QWidget *parent = nullptr);
        void loadCards();
        QStringList processQuery(const QString *);
        QStringList getNameList();
};

#endif
