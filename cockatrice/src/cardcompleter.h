#ifndef CARDCOMPLETER_H
#define CARDCOMPLETER_H

#include <QKeyEvent>
#include <QStringList>
#include <QCompleter>

typedef QMultiHash<QString, int> TrigramIndex;

class CardNameCompleter
{
    protected:
        TrigramIndex lookupIndex;
        QStringList cardNameList;
        QCompleter internalCompleter;

    private:
        void indexName(const QString *, int);

    public:
        void loadCards();
        QStringList processQuery(const QString *);
        QStringList getNameList();
        explicit CardNameCompleter();
};

#endif
