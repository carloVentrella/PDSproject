#include "secondsettingswindow.h"
#include "ui_secondsettingswindow.h"

SecondSettingsWindow::SecondSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondSettingsWindow)
{
    ui->setupUi(this);
}

SecondSettingsWindow::~SecondSettingsWindow()
{
    delete ui;
}
