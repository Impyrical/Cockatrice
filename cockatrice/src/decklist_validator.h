#ifndef DECKLIST_VALIDATOR_H
#define DECKLIST_VALIDATOR_H

#include <QString>
#include <QVector>

#include "decklist.h"

class CardPresentValidator : public ValidationFunction
{
    public:
        bool operator()(const QString cardName) override;
};

class CardCountCleaner : public CleanerFunction
{
    public:
        void operator()(DeckList *list) override;
};

#endif
