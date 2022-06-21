#include "decklist_validator.h"

#include "decklist.h"
#include "main.h"
#include "carddatabase.h"

bool CardPresentValidator::operator()(const QString cardName)
{
    if (db->getCard(cardName).isNull()) {
        errorMessage = QString("Card not present in database: %1").arg(cardName);
        return false;
    }
    errorMessage = "";
    return true;
}

void CardCountCleaner::operator()(DeckList *list)
{
    QHash<QString, DecklistCardNode*> collectedCards;

    InnerDecklistNode *root = list->getRoot();
    for (int i = 0; i < root->size(); i++) {
        auto *node = dynamic_cast<InnerDecklistNode *>(root->at(i));
        collectedCards.clear();
        for (int j = 0; j < node->size(); j++) {
            auto *card = dynamic_cast<DecklistCardNode *>(node->at(j));
            QString cardName = card->getName();
            if (collectedCards.contains(cardName)) {
                DecklistCardNode *existing = collectedCards[cardName];
                existing->setNumber(existing->getNumber() + 1);
                list->deleteNode(card);
            } else {
                collectedCards[cardName] = card;
            }
        }
    }
}

