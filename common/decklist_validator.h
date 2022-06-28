#ifndef DECKLIST_VALIDATOR_H
#define DECKLIST_VALIDATOR_H

#include <QString>
#include <QVector>
#include "decklist.h"

class ValidationFunction
{
    protected:
        DeckList *list;
    public:
        virtual bool operator()(const QString cardname) = 0;
        void setDecklist(DeckList *list) { list = list; };
        QString errorMessage;
};

class CardPresentValidator : public ValidationFunction
{
    public:
        CardPresentValidator();
        bool operator()(const QString cardname) override;
};

class DecklistValidator
{
    public:
        DecklistValidator(DeckList *list);
        bool validateCard(QString cardName);
        void addValidationFunction(ValidationFunction *func);
        void alertFailed(QString cardName, QString error);

    protected:
        QVector<ValidationFunction *> validators;
        DeckList *list;
};

#endif
