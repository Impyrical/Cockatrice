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
#include <qcompleter.h>
#include <qnamespace.h>

CardNameCompleter::CardNameCompleter(QObject *parent) : QCompleter(db->getCardNameList(), parent)
{
}

void CardNameCompleter::processQuery(const QString query)
{
}


void BenchmarkCardCompletion() {
    using namespace std::chrono;
    qDebug() << "BENCHMARK::Running simple benchmark to compare performance of trigram completion with other";

    // First load the cards
    CardNameCompleter *trigramCompleter = new CardNameCompleter();

    auto startTime = QTime::currentTime();
    QCompleter *simpleCompleter = new QCompleter(db->getCardNameList());
    int msecs = startTime.msecsTo(QTime::currentTime());
    qDebug() << "QCompleter::loadCards Loaded and indexed in" << QString("%1ms").arg(msecs);
    simpleCompleter->setFilterMode(Qt::MatchContains);

    QString simpleQuery = "Testament of Faith";

    // Create a file to graph from
    std::ofstream resultFile;
    resultFile.open("complete_bench.csv", std::ofstream::out | std::ofstream::trunc);
    resultFile << "querylen,simple,trigram\n";
    // // Note we start at 3 since we are using trigrams
    for (int i = 3; i <= simpleQuery.length(); i++) {
        QString subquery = simpleQuery.first(i);
        auto trigramStart = high_resolution_clock::now();
        int trifound = trigramCompleter->completionModel()->rowCount();
        trigramCompleter->processQuery(subquery.toLower());
        auto trigramEnd = high_resolution_clock::now();
        auto trigramTime = duration_cast<microseconds>(trigramEnd - trigramStart).count();

        auto simpleStart = high_resolution_clock::now();
        simpleCompleter->setCompletionPrefix(subquery);
        int simpfound = simpleCompleter->completionModel()->rowCount();
        qDebug() << "When querying for" << subquery << ", simple found" << simpfound << simpleCompleter->filterMode() << "and trigram found" << trifound << "results";
        auto simpleEnd = high_resolution_clock::now();
        auto simpleTime = duration_cast<microseconds>(simpleEnd - simpleStart).count();

        resultFile << i << "," << simpleTime << "," << trigramTime << "\n";
    }
    resultFile.close();
}
