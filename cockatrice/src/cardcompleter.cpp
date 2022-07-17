#include "main.h"
#include "cardcompleter.h"
#include "carddatabase.h"

#include <bits/chrono.h>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <QStringListModel>
#include <QDebug>
#include <QTime>
#include <QString>
#include <QSet>
#include <QHash>

CardNameCompleter::CardNameCompleter(QObject *parent) : QCompleter(parent)
{
    loadCards();
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
    trigramModel = new QStringListModel(cardNameList, this);
    setModel(trigramModel);
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

void CardNameCompleter::processQuery(const QString query)
{
    QStringList trigrams;
    for (int i = 0; i < query.size(); i++) {
        trigrams.append(query.mid(i, 3));
    }
    QStringList result;
    QSet<int> candidates;
    foreach(QString trigram, trigrams) {
        foreach (int nameIndex, lookupIndex.values(trigram.toLower())) {
            if (!candidates.contains(nameIndex)) {
                QString doc = cardNameList[nameIndex];
                if (doc.contains(query, Qt::CaseInsensitive))
                    result.append(doc);
                candidates.insert(nameIndex);
            }
        }
    }
    trigramModel->setStringList(result);
}

NaiveCardCompleter::NaiveCardCompleter(QObject *parent) : QCompleter(parent)
{
    loadCards();
}

void NaiveCardCompleter::loadCards()
{
    qDebug() << "Naive card completer loading cards";
    auto startTime = QTime::currentTime();
    int count = 0;
    foreach (QString cardName, db->getCardNameList()) {
        cardNameList.append(cardName);
        count++;
    }
    int msecs = startTime.msecsTo(QTime::currentTime());
    qDebug() << "NaiveCardCompleter::loadCards Loaded and indexed " << cardNameList.size() << "cards in" << QString("%1ms").arg(msecs);
}

void NaiveCardCompleter::processQuery(const QString query)
{
    setCompletionPrefix(query);
    completionModel();
}

void BenchmarkCardCompletion() {
    using namespace std::chrono;
    qDebug() << "BENCHMARK::Running simple benchmark to compare performance of trigram completion with other";

    // First load the cards
    CardNameCompleter *trigramCompleter = new CardNameCompleter();
    NaiveCardCompleter *simpleCompleter = new NaiveCardCompleter();

    QString simpleQuery = "Testament of Faith";

    // Create a file to graph from
    std::ofstream resultFile;
    resultFile.open("complete_bench.csv", std::ofstream::out | std::ofstream::trunc);
    resultFile << "querylen,simple,trigram\n";
    // // Note we start at 3 since we are using trigrams
    for (int i = 3; i < simpleQuery.length(); i++) {
        QString subquery = simpleQuery.first(i);
        auto trigramStart = high_resolution_clock::now();
        trigramCompleter->processQuery(subquery);
        auto trigramEnd = high_resolution_clock::now();
        auto trigramTime = duration_cast<microseconds>(trigramEnd - trigramStart).count();

        auto simpleStart = high_resolution_clock::now();
        simpleCompleter->processQuery(subquery);
        auto simpleEnd = high_resolution_clock::now();
        auto simpleTime = duration_cast<microseconds>(simpleEnd - simpleStart).count();
        resultFile << i << "," << simpleTime << "," << trigramTime << "\n";
    }
    resultFile.close();
}
