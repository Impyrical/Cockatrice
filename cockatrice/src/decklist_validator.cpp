#include "decklist_validator.h"

#include "main.h"
#include "carddatabase.h"

#include <QDebug>

CardPresentValidator::CardPresentValidator()
{
}

bool CardPresentValidator::operator()(const QString cardName)
{
    if (db->getCard(cardName).isNull()) {
        errorMessage = QString("Card not present in database: %1").arg(cardName);
        return false;
    }
    errorMessage = "";
    return true;
}
