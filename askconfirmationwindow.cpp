#include "askconfirmationwindow.h"
#include "ui_askconfirmationwindow.h"

AskConfirmationWindow::AskConfirmationWindow(QString username,int dimension, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskConfirmationWindow)
{
    ui->setupUi(this);

    //label for the username of the sender
    ui->nameLabel->setText(username);
    ui->nameLabel->text().append("?");

    int unity=0;
    QString measure;
    while(dimension/1000!=0)
    {
        dimension=dimension/1000;
        unity++;
    }
    switch(unity){
    case 3:
        measure="GB";
        break;
    case 2:
        measure="MB";
        break;
    case 1:
        measure="KB";
        break;
    case 0:
        measure="B";
    default:
        measure="B";
    }

    //label for the dimension of the  files
    ui->dimensionLabel->setText("("+QString::number(dimension)+measure+")"); //TO BE SET: unity of measure (GB? MB?)

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
    emit cannotBeReceived();
    this->hide();
}
