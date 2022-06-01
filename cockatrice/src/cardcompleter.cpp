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
}

void CardNameCompleter::indexName(const QString *cardName, int index) {
    QStringList trigrams;
    for (int i = 0; i < cardName->size()-3; i++) {
        trigrams.append(cardName->mid(i, 3).toLower());
    }
    foreach(QString trigram, trigrams) {
        lookupIndex.insert(trigram, index);
    }
}

QStringList CardNameCompleter::processQuery(const QString *query)
{
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
