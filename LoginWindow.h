#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include "SystemManager.h"

class LoginWindow : public QDialog {
    Q_OBJECT
public:
    LoginWindow(SystemManager* manager, QWidget* parent = nullptr);

signals:
    void userLoggedIn(User* user);

private slots:
    void handleLogin();
    void handleSignup();

private:
    SystemManager* systemManager;
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLabel* statusLabel;
};

#endif // LOGINWINDOW_H
