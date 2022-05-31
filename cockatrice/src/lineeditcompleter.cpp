#include "lineeditcompleter.h"

#include <QObject>
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

void LineEditCompleter::moveCompleter(int offset)
{
    QCompleter *completer = cardCompleter->popup()->isVisible() ? cardCompleter : c;
    int row = completer->currentRow();
    QItemSelectionModel *sm = new QItemSelectionModel(completer->completionModel());
    completer->popup()->setSelectionModel(sm);
    QModelIndex targetIndex = offset >= 0 ? completer->completionModel()->index(0, 0) : completer->completionModel()->index(completer->completionCount()-1, 0);
    if (completer->completionModel()->hasIndex(row+offset, 0)) {
        targetIndex = completer->completionModel()->index(row + offset, 0);
    }
    completer->setCurrentRow(targetIndex.row());
    sm->select(targetIndex, QItemSelectionModel::ClearAndSelect);
    sm->setCurrentIndex(targetIndex, QItemSelectionModel::NoUpdate);
}

bool LineEditCompleter::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride || event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            moveCompleter(1);
        } else if (keyEvent->key() == Qt::Key_Backtab) {
            moveCompleter(-1);
        } else if (keyEvent->key() == Qt::Key_Shift) {
            if (cardCompleter->popup()->isVisible()) {
                return true;
            }
        }
    }
    return QWidget::event(event);
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
    moveCompleter(0);
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
