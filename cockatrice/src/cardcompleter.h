#ifndef CARDCOMPLETER_H
#define CARDCOMPLETER_H

#include <QKeyEvent>
#include <QStringList>
#include <QStringListModel>
#include <QCompleter>
#include <qtmetamacros.h>

typedef QMultiHash<QString, int> TrigramIndex;

class CardNameCompleter : public QCompleter
{
    Q_OBJECT
    protected:
        TrigramIndex lookupIndex;
        QStringList cardNameList;

    private:
        void indexName(const QString *, int);
        QStringListModel *trigramModel;

    public:
        explicit CardNameCompleter(QObject *parent = nullptr);
        void loadCards();
        QStringList processQuery(const QString *);
};

class NaiveCardCompleter : public QCompleter
{
    Q_OBJECT
    protected:
        QStringList cardNameList;
    public:
        explicit NaiveCardCompleter(QObject *parent = nullptr);
        void loadCards();
};

void BenchmarkCardCompletion();

#endif
