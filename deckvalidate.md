# Supporting Deck list Validation on Load

Goal: make it so that we trigger an error/alert when an invalid deck is loaded.

Referenced Issues:

- [4019](https://github.com/Cockatrice/Cockatrice/issues/4019)
- [2951](https://github.com/Cockatrice/Cockatrice/issues/2951)

## Defining Invalid

I think to start with the criteria for an invalid decklist should simply be a card not being present in the currently loaded card database. That will give a good starting point that we can add additional criteria to in the future.

## Deck Cleaning

A sub goal here is to clean up the decks that get loaded so that they neatly fit into our format. Issue #2951 for example notes that having the same card on repeated lines doesn't collapse nicely. This would be easy to support. Again, we can add additional cleaning capabilities in the future, but for now that's probably a good starting point.

## Important Files

- decklist.cpp has all the loading and saving logic that'll need to be touched
- dlg_load_deck_from_clipboard drives loading from clipboard
- deck_loader also has important logic

The most important piece in all of this is loadFromStream_Plain in common/decklist.cpp. This function goes through and creates all the decklist nodes that we might need.
## Plan of Attack

Probably the right thing to do is build an alert system around the current loader that we can easily add criteria to. That seems future-useful for supporting deck validation for specific formats for instance.

I /think/ that the right way of going about a validator is to have a class with a function that returns a boolean when given a decklist and a potential node for the decklist. This allows us to build both a cleaner and a validator: it can modify the decklist in place if the new node really requries a clean, or it can simply return false as an error (or return an error type). I'll probably put this in a separate file for now, for convenience's sake.

For cleaning, I'm not totally sure. Perhaps a generic cleaner object that takes a set of rules and applies them to the deck as it's loaded? That seems reasonably elegant.
