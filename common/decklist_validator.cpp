#include "decklist_validator.h"
#include "decklist.h"
#include <QDebug>

CardPresentValidator::CardPresentValidator()
{
}

bool CardPresentValidator::operator()(const QString cardname)
{
    qDebug() << "Checking if " << cardname << "is valid";
    return true;
}

DecklistValidator::DecklistValidator(DeckList *list)
    : list(list)
{
}

void DecklistValidator::addValidationFunction(ValidationFunction *func)
{
    func->setDecklist(list);
    validators.append(func);
}

bool DecklistValidator::validateCard(QString cardName)
{
    qDebug() << "Validating card" << cardName;
    for (ValidationFunction *v : validators) {
        if (!(*v)(cardName)) {
            return false;
        }
    }
    return true;
}

void DecklistValidator::alertFailed(QString cardName, QString error)
{
}
