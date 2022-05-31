#include "main.h"
#include "cardcompleter.h"
#include "carddatabase.h"

#include <QStringListModel>
#include <QDebug>
#include <QTime>
#include <QString>
#include <QSet>
#include <QHash>

CardNameCompleter::CardNameCompleter()
{
}

void CardNameCompleter::loadCards()
{
    /*
     * This requires some thought I think. Obviously we want to start by just grabbing the list of all the card names from the db
     * However, what do we do with them? Should we feed them to a completer? That seems limited. Perhaps to start with that's fine though
     */
    qDebug() << "CardCompleter::loadCards Loading cards for the card name completer";

    auto startTime = QTime::currentTime();
    int count = 0;
    foreach (QString cardName, db->getCardNameList()) {
        cardNameList.append(cardName);
        indexName(&cardName, count);
        count++;
    }
    int msecs = startTime.msecsTo(QTime::currentTime());
    qDebug() << "CardCompleter::loadCards Loaded and indexed " << cardNameList.size() << "cards in" << QString("%1ms").arg(msecs);

    qDebug() << "Doing some test queries on the list of cards";

    QStringList testQueries = {"jace", "chandra", "sliver"};

    foreach(QString query, testQueries) {
        QStringList queryResult = processQuery(&query);
        qDebug() << "CardCompleter::TestQueries querying for " << query << " produced " << queryResult;
    }
}

void CardNameCompleter::indexName(const QString *cardName, int index) {
    // First get the trigrams of the string
    QStringList trigrams;
    for (int i = 0; i < cardName->size()-3; i++) {
        trigrams.append(cardName->mid(i, 3).toLower());
    }
    // qDebug() << "CardNameCompleter::indexName " << *cardName << " becomes " << trigrams;
    foreach(QString trigram, trigrams) {
        lookupIndex.insert(trigram, index);
    }
    // qDebug() << "CardNameCompleter::indexName after inserting " << *cardName << 
    //     " has first trigram index " << lookupIndex.values(trigrams[0]);
}

QStringList CardNameCompleter::processQuery(const QString *query)
{
    qDebug() << "CardCompleter::processQuery processing query " << *query << " using trigram index";
    QStringList trigrams;
    for (int i = 0; i < query->size(); i++) {
        trigrams.append(query->mid(i, 3));
    }
    QStringList result;
    QSet<int> candidates;
    foreach(QString trigram, trigrams) {
        foreach (int nameIndex, lookupIndex.values(trigram.toLower())) {
            if (!candidates.contains(nameIndex)) {
                QString doc = cardNameList[nameIndex];
                if (doc.contains(*query, Qt::CaseInsensitive))
                    result.append(doc);
                candidates.insert(nameIndex);
            }
        }
    }
    return result;
}

QStringList CardNameCompleter::getNameList()
{
    QStringList result = { "foo", "bar", "baz" };
    qDebug() << "CardCompleter::getNameList Returning the list of cards used for card name completion";
    return result;
}
