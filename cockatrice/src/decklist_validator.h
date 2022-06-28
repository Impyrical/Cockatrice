#ifndef DECKLIST_VALIDATOR_H
#define DECKLIST_VALIDATOR_H

#include <QString>
#include <QVector>

#include "decklist.h"

class CardPresentValidator : public ValidationFunction
{
    public:
        CardPresentValidator();
        bool operator()(const QString cardName) override;
};

#endif
