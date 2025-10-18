#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SarahaApplication1.h"

class SarahaApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    SarahaApplication1(QWidget *parent = nullptr);
    ~SarahaApplication1();

private:
    Ui::SarahaApplication1Class ui;
};
