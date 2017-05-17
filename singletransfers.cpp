#include "singletransfers.h"
#include "ui_singletransfers.h"

SingleTransfers::SingleTransfers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleTransfers)
{
    ui->setupUi(this);
}

SingleTransfers::~SingleTransfers()
{
    delete ui;
}
