#ifndef FAVORITESDIALOG_H
#define FAVORITESDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QString>
#include <QVBoxLayout>
#include <QListWidget>

class FavoritesDialog : public QDialog {
    Q_OBJECT

public:
    explicit FavoritesDialog(QWidget* parent = nullptr);

    void setDisplayText(const QString& text);
    void showAddToFavoritesButton(bool visible);
    void showDeleteOldestButton(bool visible);
    void setMessages(const QStringList& messages);
    QString getSelectedMessage()const;

signals:
    void addToFavoritesClicked();
    void deleteOldestClicked();

private:
    QTextEdit* displayArea;
    QListWidget* messageList;
    QPushButton* addToFavoritesButton;
    QPushButton* deleteOldestButton;
};

#endif // FAVORITESDIALOG_H
