#include "FavoritesDialog.h"

FavoritesDialog::FavoritesDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Favorites");

    messageList = new QListWidget(this);
    messageList->setSelectionMode(QAbstractItemView::SingleSelection);

    addToFavoritesButton = new QPushButton("Add to Favorites", this);
    deleteOldestButton = new QPushButton("Delete Oldest Favorite", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(messageList, 1);
    layout->addWidget(displayArea);
    layout->addWidget(addToFavoritesButton);
    layout->addWidget(deleteOldestButton);

    connect(addToFavoritesButton, &QPushButton::clicked, this, &FavoritesDialog::addToFavoritesClicked);
    connect(deleteOldestButton, &QPushButton::clicked, this, &FavoritesDialog::deleteOldestClicked);
    resize(400, 300);
    setMinimumSize(300, 200);
}

void FavoritesDialog::setDisplayText(const QString& text) {
    QStringList messages = text.split("\n", Qt::SkipEmptyParts);
    setMessages(messages);
}

void FavoritesDialog::setMessages(const QStringList& messages) {
    messageList->clear();
    for (const QString& message : messages) {
        messageList->addItem(message);
    }
}

void FavoritesDialog::showAddToFavoritesButton(bool visible) {
    addToFavoritesButton->setVisible(visible);
}

void FavoritesDialog::showDeleteOldestButton(bool visible) {
    deleteOldestButton->setVisible(visible);
}

QString FavoritesDialog::getSelectedMessage() const {
    QListWidgetItem* selectedItem = messageList->currentItem();
    return selectedItem ? selectedItem->text() : QString();
}
