#include "presettings.h"
#include "ui_presettings.h"

PreSettings::PreSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreSettings)
{
    ui->setupUi(this);
}

PreSettings::~PreSettings()
{
    delete ui;
}
