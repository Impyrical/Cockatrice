#include "lineeditcompleter.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextCursor>
#include <QWidget>
#include <QApplication>

LineEditCompleter::LineEditCompleter(QWidget *parent) : LineEditUnfocusable(parent), c(nullptr)
{
    cardCompleter = new CardNameCompleter(this);
    cardCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    cardCompleter->setMaxVisibleItems(10);
    cardCompleter->setFilterMode(Qt::MatchStartsWith);
    cardCompleter->setWidget(this);
    cardCompleter->popup()->setTabKeyNavigation(false);

    connect(cardCompleter, SIGNAL(activated(QString)), this, SLOT(insertCardCompletion(QString)));
}

void LineEditCompleter::focusOutEvent(QFocusEvent *e)
{
    LineEditUnfocusable::focusOutEvent(e);
    if (c->popup()->isVisible()) {
        // Remove Popup
        c->popup()->hide();
        // Truncate the line to last space or whole string
        QString textValue = text();
        int lastIndex = textValue.length();
        int lastWordStartIndex = textValue.lastIndexOf(" ") + 1;
        int leftShift = qMin(lastIndex, lastWordStartIndex);
        setText(textValue.left(leftShift));
        // Insert highlighted line from popup
        insert(c->completionModel()->index(c->popup()->currentIndex().row(), 0).data().toString() + " ");
        // Set focus back to the textbox since tab was pressed
        setFocus();
    }
}

void LineEditCompleter::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Escape:
            if (c->popup()->isVisible()) {
                event->ignore();
                // Remove Popup
                c->popup()->hide();
                // Truncate the line to last space or whole string
                QString textValue = text();
                int lastIndexof = qMax(0, textValue.lastIndexOf(" "));
                QString finalString = textValue.left(lastIndexof);
                // Add a space if there's a word
                if (finalString != "")
                    finalString += " ";
                setText(finalString);
                return;
            } else if (cardCompleter->popup()->isVisible()) {
                event->ignore();
                cardCompleter->popup()->hide();
                // Clear out all the text for the card completion
                QString before = text().left(lastDoubleBracketIndex).trimmed();
                QString after = text().mid(cursorPosition()).trimmed();
                // Set the cursor to where the completion would have been
                setText(before + " " + after);
                setCursorPosition(before.size() + 1);
                return;
            }
            break;
        case Qt::Key_Space:
            if (c->popup()->isVisible()) {
                event->ignore();
                // Remove Popup
                c->popup()->hide();
                // Truncate the line to last space or whole string
                QString textValue = text();
                int lastIndex = textValue.length();
                int lastWordStartIndex = textValue.lastIndexOf(" ") + 1;
                int leftShift = qMin(lastIndex, lastWordStartIndex);
                setText(textValue.left(leftShift));
                // Insert highlighted line from popup
                insert(c->completionModel()->index(c->popup()->currentIndex().row(), 0).data().toString() + " ");
                return;
            } 
            // NOTE: Do not do anything for the card completer. We want to allow people to input spaces
            break;
        case Qt::Key_Tab:
            qDebug("Tab key pressed");
            return;
        default:
            break;
    }

    LineEditUnfocusable::keyPressEvent(event);

    QString beforeCursor = text().left(cursorPosition());
    lastDoubleBracketIndex = beforeCursor.lastIndexOf("[[");
    cardCompleter->popup()->hide();
    if (lastDoubleBracketIndex != -1 && lastDoubleBracketIndex > beforeCursor.lastIndexOf("]]")) {
        // This means that we're inside some brackets, now check if we have a trigram
        int queryLen = beforeCursor.size() - lastDoubleBracketIndex - 2;
        if (queryLen >= 3) {
            // Get the actual query
            QString query = beforeCursor.right(queryLen);
            QStringList queryResults = cardCompleter->processQuery(&query);
            QRect cr = cursorRect();
            cr.setWidth(cardCompleter->popup()->sizeHintForColumn(0) +
                    cardCompleter->popup()->verticalScrollBar()->sizeHint().width());
            cardCompleter->complete(cr); 

            QItemSelectionModel *sm = new QItemSelectionModel(cardCompleter->completionModel());
            cardCompleter->popup()->setSelectionModel(sm);
            sm->select(cardCompleter->completionModel()->index(0, 0), QItemSelectionModel::ClearAndSelect);
            sm->setCurrentIndex(cardCompleter->completionModel()->index(0, 0), QItemSelectionModel::NoUpdate);
        }
    }
    // return if the completer is null or if the most recently typed char was '@'.
    // Only want the popup AFTER typing the first char of the mention.
    if (!c || text().right(1).contains("@")) {
        c->popup()->hide();
        return;
    }

    // Set new completion prefix
    c->setCompletionPrefix(cursorWord(text()));
    if (c->completionPrefix().length() < 1) {
        c->popup()->hide();
        return;
    }

    // Draw completion box
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr);

    // Select first item in the completion popup
    QItemSelectionModel *sm = new QItemSelectionModel(c->completionModel());
    c->popup()->setSelectionModel(sm);
    sm->select(c->completionModel()->index(0, 0), QItemSelectionModel::ClearAndSelect);
    sm->setCurrentIndex(c->completionModel()->index(0, 0), QItemSelectionModel::NoUpdate);
}

QString LineEditCompleter::cursorWord(const QString &line) const
{
    return line.mid(line.left(cursorPosition()).lastIndexOf(" ") + 1,
                    cursorPosition() - line.left(cursorPosition()).lastIndexOf(" ") - 1);
}

void LineEditCompleter::insertCompletion(QString arg)
{
    QString s_arg = arg + " ";
    setText(text().replace(text().left(cursorPosition()).lastIndexOf(" ") + 1,
                           cursorPosition() - text().left(cursorPosition()).lastIndexOf(" ") - 1, s_arg));
}

void LineEditCompleter::insertCardCompletion(QString arg)
{
    QString beforeCompletion = text().left(lastDoubleBracketIndex).trimmed();
    QString afterCompletion = text().mid(cursorPosition() - 1).trimmed();
    QString completion = QString("[[%1]]").arg(arg);
    setText(beforeCompletion + " " + completion + " " + afterCompletion);
}

void LineEditCompleter::setCompleter(QCompleter *completer)
{
    c = completer;
    c->setWidget(this);
    connect(c, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void LineEditCompleter::setCompletionList(QStringList completionList)
{
    if (!c || c->popup()->isVisible())
        return;

    QStringListModel *model;
    model = (QStringListModel *)(c->model());
    if (model == NULL)
        model = new QStringListModel();
    model->setStringList(completionList);
}
