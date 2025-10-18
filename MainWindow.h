#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtCore/QQueue>
#include <QString> 
#include "SystemManager.h"
#include "FavoritesDialog.h"
#include<tuple>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(SystemManager* manager, User* currentUser, QWidget* parent = nullptr);

private slots:
    void SendMessage();
    void ViewSentMessages();
    void ViewReceivedMessages();
    void undoLastMessage();
    void DisplaySortedContacts();
    void removeContact();
    void searchContact();
    void viewFavoriteMessages();
    void handleAddToFavorites();
    void handleDeleteOldestFavorite();
    void openFavoritesDialog();
    void ReceiveMessage();
    void AddContact();


private:
    SystemManager* systemManager;
    User* currentUser;
    User* otherUser; // For demo purposes
    QLineEdit* contactIdInput;
    QTextEdit* messageInput;
    QTextEdit* displayArea;
    QQueue<QString>favoriteMessages;

};