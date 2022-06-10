#ifndef CARDCOMPLETER_H
#define CARDCOMPLETER_H

#include <QKeyEvent>
#include <QStringList>
#include <QStringListModel>
#include <QCompleter>

typedef QMultiHash<QString, int> TrigramIndex;

class CardNameCompleter : public QCompleter
{
    Q_OBJECT
    protected:
        TrigramIndex lookupIndex;
        QStringList cardNameList;
        bool event(QEvent *);

    private:
        void indexName(const QString *, int);
        QStringListModel *trigramModel;

    public:
        explicit CardNameCompleter(QObject *parent = nullptr);
        void loadCards();
        QStringList processQuery(const QString *);
};

#endif
