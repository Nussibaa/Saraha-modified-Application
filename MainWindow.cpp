#include "MainWindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/qmessagebox>
#include<tuple>


MainWindow::MainWindow(SystemManager* manager, User* currentUser, QWidget* parent)
    : QMainWindow(parent), systemManager(manager), currentUser(currentUser) {
    setWindowTitle("Saraha Application");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* idLayout = new QHBoxLayout();
    QLabel* idLabel = new QLabel("Contact ID:", this);
    contactIdInput = new QLineEdit(this);
    idLayout->addWidget(idLabel);
    idLayout->addWidget(contactIdInput);
    mainLayout->addLayout(idLayout);

    QHBoxLayout* msgLayout = new QHBoxLayout();
    QLabel* msgLabel = new QLabel("Message:", this);
    messageInput = new QTextEdit(this);
    messageInput->setFixedHeight(50);
    msgLayout->addWidget(msgLabel);
    msgLayout->addWidget(messageInput);
    mainLayout->addLayout(msgLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* sendButton = new QPushButton("Send Message", this);
    QPushButton* viewSentButton = new QPushButton("View Sent", this);
    QPushButton* viewReceivedButton = new QPushButton("View Received", this);
    QPushButton* undoButton = new QPushButton("Undo Last Message", this);
    QPushButton* DisplayContactsSorted = new QPushButton("Display contacts sorted", this);
    QPushButton* SearchContact = new QPushButton("Search Contact", this);
    QPushButton* RemoveContact = new QPushButton("Remove Contact", this);
    QPushButton* viewFavoritesButton = new QPushButton("View Favorites", this);
    QPushButton* receiveMessageButton = new QPushButton(" Receive message", this);
    QPushButton* addContactButton = new QPushButton("Add Contact", this);

    // Set the button styles
    QString buttonStyle = "QPushButton {"
        "background-color: #2c3e50;"
        "color: white;"
        "font-size: 16px;"
        "padding: 10px;"
        "border: none;"
        "border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "background-color: #34495e;"
        "}";

    sendButton->setStyleSheet(buttonStyle);
    viewSentButton->setStyleSheet(buttonStyle);
    viewReceivedButton->setStyleSheet(buttonStyle);
    undoButton->setStyleSheet(buttonStyle);
    DisplayContactsSorted->setStyleSheet(buttonStyle);
    SearchContact->setStyleSheet(buttonStyle);
    RemoveContact->setStyleSheet(buttonStyle);
    viewFavoritesButton->setStyleSheet(buttonStyle);
    receiveMessageButton->setStyleSheet(buttonStyle);
    addContactButton->setStyleSheet(buttonStyle);

    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(viewSentButton);
    buttonLayout->addWidget(viewReceivedButton);
    buttonLayout->addWidget(undoButton);
    buttonLayout->addWidget(DisplayContactsSorted);
    buttonLayout->addWidget(SearchContact);
    buttonLayout->addWidget(RemoveContact);
    buttonLayout->addWidget(viewFavoritesButton);
    buttonLayout->addWidget(receiveMessageButton);
    mainLayout->addLayout(buttonLayout);
    buttonLayout->addWidget(addContactButton);



    displayArea = new QTextEdit(this);
    displayArea->setReadOnly(true);
    mainLayout->addWidget(displayArea);

    connect(sendButton, &QPushButton::clicked, this, &MainWindow::SendMessage);
    connect(viewSentButton, &QPushButton::clicked, this, &MainWindow::ViewSentMessages);
    connect(viewReceivedButton, &QPushButton::clicked, this, &MainWindow::ViewReceivedMessages);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::undoLastMessage);
    connect(DisplayContactsSorted, &QPushButton::clicked, this, &MainWindow::DisplaySortedContacts);
    connect(SearchContact, &QPushButton::clicked, this, &MainWindow::searchContact);
    connect(RemoveContact, &QPushButton::clicked, this, &MainWindow::removeContact);
    connect(viewFavoritesButton, &QPushButton::clicked, this, &MainWindow::viewFavoriteMessages);
    connect(addContactButton, &QPushButton::clicked, this, &MainWindow::AddContact);
    connect(receiveMessageButton, &QPushButton::clicked, this, &MainWindow::ReceiveMessage);

}

void MainWindow::SendMessage() {
    // Validate inputs
    bool ok;
    int contactId = contactIdInput->text().toInt(&ok);
    if (!ok || contactId <= 0) {
        displayArea->setText("Please enter a valid Contact ID.");
        return;
    }
    QString message = messageInput->toPlainText();
    if (message.isEmpty()) {
        displayArea->setText("Please enter a message.");
        return;
    }
    int expirationHours = 24;
    time_t expirationTime = time(nullptr) + expirationHours * 3600;

    // Check recipient existence
    User* recipient = systemManager->getUserByID(contactId);
    if (!recipient) {
        displayArea->setText("Contact ID not found!");
        return;
    }

    // Format the message consistently for both sender and recipient
    string timestamp = currentUser->getTimestamp();
    string formattedMessage = "[" + timestamp + "] To ID: " + to_string(contactId) + "\nMessage: " + message.toStdString() + "\n";

    // Send message and update recipient
    string result = currentUser->sendMessage(contactId, formattedMessage, expirationHours);
    try {
        string receiveResult = recipient->ReceiveMessage(currentUser->getID(), formattedMessage, expirationTime);
        displayArea->setText(QString::fromStdString(result));
        if (result == "Message is sent and will expire in 24 hours.\n") {
            messageInput->clear();
            contactIdInput->clear();
        }
    }
    catch (const std::exception& e) {
        currentUser->undoMessage();
        displayArea->setText("Failed to deliver message: " + QString::fromStdString(e.what()));
    }
}
void MainWindow::ViewSentMessages() {
    const auto& messages = currentUser->viewSentMessages();
    QString displayText = "Sent Messages:\n";

    for (const auto& msgTuple : messages) {
        const string& msgText = std::get<0>(msgTuple);
        // You could also display expiration time if needed using std::get<1>(msgTuple)
        displayText += QString::fromStdString(msgText) + "\n";
    }

    displayArea->setText(displayText);
}


void MainWindow::ViewReceivedMessages() {
    FavoritesDialog* dialog = new FavoritesDialog(this);
    dialog->setDisplayText("Here are the received messages...");
    int contactId = contactIdInput->text().toInt();
    if (contactId == 0) {
        displayArea->setText("Please enter a valid Contact ID.");
        return;
    }
    vector<pair<string, time_t>> messages = currentUser->viewReceivedMessages(contactId);
    QStringList messageList;
    messageList << QString("Received Messages from ID %1:").arg(contactId);

    if (messages.empty()) {
        messageList << "No messages found.";
    }
    else {
        for (const auto& msgPair : messages) {
            messageList << QString::fromStdString(msgPair.first);
        }
    }

    dialog->setMessages(messageList);
    dialog->showAddToFavoritesButton(true);
    dialog->showDeleteOldestButton(false);

    connect(dialog, &FavoritesDialog::addToFavoritesClicked, this, &MainWindow::handleAddToFavorites);

    dialog->exec();
}



void MainWindow::undoLastMessage() {
    if (currentUser->viewSentMessages().empty()) {
        displayArea->setText("No message to undo.");
        return;
    }

    // Get the last sent message
    auto lastMessage = currentUser->viewSentMessages().back();
    int receiverId = get<2>(lastMessage);
    string messageText = get<0>(lastMessage);
    time_t expirationTime = get<1>(lastMessage);

    // Undo the sender's message
    string result = currentUser->undoMessage();

    // Remove the corresponding received message
    User* recipient = systemManager->getUserByID(receiverId);
    if (recipient) {
        string removeResult = recipient->removeReceivedMessage(currentUser->getID(), messageText, expirationTime);
        displayArea->setText(QString::fromStdString(result + removeResult));
    }
    else {
        displayArea->setText(QString::fromStdString(result + "Recipient not found, sender's message undone."));
    }
}

void MainWindow::DisplaySortedContacts() {
    string sortedContacts = currentUser->DisplaySortContactsByMessages();
    displayArea->setText(QString::fromStdString(sortedContacts));
}

void MainWindow::removeContact() {
    int contactId = contactIdInput->text().toInt();
    if (contactId == 0) {
        displayArea->setText("Please enter a valid Contact ID.");
        return;
    }
    std::string result = currentUser->RemoveContact(contactId);
    displayArea->setText(QString::fromStdString(result));
}

void MainWindow::searchContact() {
    int contactId = contactIdInput->text().toInt();
    if (contactId == 0) {
        displayArea->setText("Please enter a valid Contact ID.");
        return;
    }
    string result = currentUser->searchContact(contactId);
    displayArea->setText(QString::fromStdString(result));
}

void MainWindow::viewFavoriteMessages() {
    FavoritesDialog* dialog = new FavoritesDialog(this);

    // Get the favorite messages
    queue<string> tempFavorites = currentUser->viewFavorites();

    if (tempFavorites.empty()) {
        QStringList messages;
        messages << "No favorite messages to display.";
        dialog->setMessages(messages);
        dialog->showAddToFavoritesButton(false);
        dialog->showDeleteOldestButton(false);
    }
    else {
        QStringList messageList;
        messageList << "Favorite Messages:";
        int counter = 1;
        while (!tempFavorites.empty()) {
            messageList << QString("%1. %2").arg(counter++).arg(QString::fromStdString(tempFavorites.front()));
            tempFavorites.pop();
        }
        dialog->setMessages(messageList);
        dialog->showAddToFavoritesButton(false);
        dialog->showDeleteOldestButton(true);
    }

    // Connect signals
    connect(dialog, &FavoritesDialog::addToFavoritesClicked, this, &MainWindow::handleAddToFavorites);
    connect(dialog, &FavoritesDialog::deleteOldestClicked, this, &MainWindow::handleDeleteOldestFavorite);

    dialog->exec();
}

void MainWindow::handleAddToFavorites() {
    FavoritesDialog* dialog = qobject_cast<FavoritesDialog*>(sender());
    if (!dialog) {
        QMessageBox::warning(this, "Error", "Unable to retrieve selected message.");
        return;
    }

    QString selectedMessage = dialog->getSelectedMessage();
    if (selectedMessage.isEmpty()) {
        QMessageBox::information(this, "Info", "Please select a message to add to favorites.");
        return;
    }

    // Remove any header like "Received Messages from ID X:" if present
    if (selectedMessage.startsWith("Received Messages from ID")) {
        QMessageBox::information(this, "Info", "Please select a message, not the header.");
        return;
    }

    currentUser->addMessageToFav(selectedMessage.toStdString());
    QMessageBox::information(this, "Success", "Message added to favorites.");
}

void MainWindow::handleDeleteOldestFavorite() {
    string result = currentUser->removeOldestFav();
    QMessageBox::information(this, "Favorite Deleted", QString::fromStdString(result));
}

void MainWindow::openFavoritesDialog() {
    FavoritesDialog* dialog = new FavoritesDialog(this);

    connect(dialog, &FavoritesDialog::addToFavoritesClicked, this, &MainWindow::handleAddToFavorites);
    connect(dialog, &FavoritesDialog::deleteOldestClicked, this, &MainWindow::handleDeleteOldestFavorite);

    dialog->exec();

}

void MainWindow::ReceiveMessage() {
    // Validate inputs
    bool ok;
    int senderId = contactIdInput->text().toInt(&ok);
    if (!ok || senderId <= 0) {
        displayArea->setText("Please enter a valid Sender ID.");
        return;
    }
    QString message = messageInput->toPlainText();
    if (message.isEmpty()) {
        displayArea->setText("Please enter a message.");
        return;
    }
    time_t expirationTime = time(nullptr) + 24 * 3600;
    string result = currentUser->ReceiveMessage(senderId, message.toStdString(), expirationTime);

    // Display the result (no prompt handling needed)
    displayArea->setText(QString::fromStdString(result));

    // Clear input fields
    contactIdInput->clear();
    messageInput->clear();
}

void MainWindow::AddContact() {
    bool ok;
    int senderId = contactIdInput->text().toInt(&ok);
    if (!ok || senderId <= 0) {
        displayArea->setText("Please enter a valid Sender ID.");
        return;
    }

    auto sender = systemManager->getUserByID(senderId);
    if (!sender) {
        displayArea->setText("Sender ID does not exist.");
        return;
    }

    if (currentUser->addContact(senderId)) {
        displayArea->setText("Added contact with ID " + QString::number(senderId) + " to your contacts.");
    }
    else {
        // Check if the contact is already in savedContacts to provide a precise message
        auto savedContacts = currentUser->getContacts();
        if (savedContacts.find(senderId) != savedContacts.end()) {
            displayArea->setText("Contact with ID " + QString::number(senderId) + " is already in your contacts.");
        }
        else {
            displayArea->setText("Failed to add contact with ID " + QString::number(senderId) + ".");
        }
    }

    contactIdInput->clear();
}
