#include "askconfirmationwindow.h"
#include "ui_askconfirmationwindow.h"

AskConfirmationWindow::AskConfirmationWindow(QString username, int numOfFiles, int dimension, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskConfirmationWindow)
{
    ui->setupUi(this);

    //label for the username of the sender
    ui->nameLabel->setText(username);
    //label for the number of files to be received
    ui->numOfFiles->setText(QString::number(numOfFiles));
    //label for the dimension of the  files
    ui->dimensionLabel->setText(QString::number(dimension)); //TO BE SET: unity of measure (GB? MB?)

    setWindowTitle("Confirm: Y | N");

    //to remove the close button in the title bar
    setWindowFlags(Qt::Window
    | Qt::WindowMinimizeButtonHint
    | Qt::WindowMaximizeButtonHint);
}

AskConfirmationWindow::~AskConfirmationWindow()
{
    delete ui;
}

void AskConfirmationWindow::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible()) {
                return;
            }
    hide();
    event->ignore();
}

void AskConfirmationWindow::on_YesButton_clicked()
{
    emit canBeReceived();
    this->hide();
}

void AskConfirmationWindow::on_NoButton_clicked()
{
    this->hide();
}
