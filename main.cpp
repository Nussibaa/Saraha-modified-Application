#include <QtWidgets/QApplication>
#include "LoginWindow.h"
#include "mainwindow.h"
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    SystemManager systemManager;
    systemManager.loadFromJson();
    LoginWindow loginWindow(&systemManager);
    MainWindow* mainWindow = nullptr;

    QObject::connect(&loginWindow, &LoginWindow::userLoggedIn, [&mainWindow, &systemManager](User* user) {
        mainWindow = new MainWindow(&systemManager, user);
        mainWindow->resize(1000, 600);
        mainWindow->show();
        });
    //Save when app is about to quit
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]() {
        systemManager.saveAllDataToJson();
        });
    loginWindow.exec();
    systemManager.saveAllDataToJson();
    return app.exec();
}
