#include "LoginWindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

LoginWindow::LoginWindow(SystemManager* manager, QWidget* parent)
    : QDialog(parent), systemManager(manager) {
    setWindowTitle("Login/Signup");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPixmap("SARAHA.jpg");  // Adjust path if needed
    logoLabel->setPixmap(logoPixmap);
    logoLabel->setAlignment(Qt::AlignCenter); // Center the image
    logoLabel->setScaledContents(true); // Scales image to fit label
    logoLabel->setFixedHeight(150); // Adjust height as needed
    mainLayout->addWidget(logoLabel);
    QHBoxLayout* usernameLayout = new QHBoxLayout();
    QLabel* usernameLabel = new QLabel("Username:", this);
    usernameInput = new QLineEdit(this);
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameInput);
    mainLayout->addLayout(usernameLayout);

    QHBoxLayout* passwordLayout = new QHBoxLayout();
    QLabel* passwordLabel = new QLabel("Password:", this);
    passwordInput = new QLineEdit(this);
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordInput);
    mainLayout->addLayout(passwordLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* loginButton = new QPushButton("Login", this);
    QPushButton* signupButton = new QPushButton("Signup", this);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(signupButton);
    mainLayout->addLayout(buttonLayout);

    statusLabel = new QLabel("", this);
    mainLayout->addWidget(statusLabel);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(signupButton, &QPushButton::clicked, this, &LoginWindow::handleSignup);
}

void LoginWindow::handleLogin() {
    string username = usernameInput->text().toStdString();
    string password = passwordInput->text().toStdString();

    if (systemManager->login(username, password)) {
        auto user = systemManager->getUser(username);
        if (user) {
            emit userLoggedIn(user);
            accept(); // Close the dialog
        }
        else {
            statusLabel->setText("Error retrieving user.");
        }
    }
    else {
        statusLabel->setText("Login failed: Invalid username or password.");
    }
}

void LoginWindow::handleSignup() {
    string username = usernameInput->text().toStdString();
    string password = passwordInput->text().toStdString();

    if (systemManager->signup(username, password)) {
        statusLabel->setText("Signup successful! Please login.");
    }
    else {
        statusLabel->setText("Signup failed: Username already exists or fields empty.");
    }
}
