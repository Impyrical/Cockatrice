#include "decklist_validator.h"
#include <QDebug>

CardPresentValidator::CardPresentValidator()
{
}

bool CardPresentValidator::operator()(const QString cardname)
{
    qDebug() << "Checking if " << cardname << "is valid";
    return true;
}
