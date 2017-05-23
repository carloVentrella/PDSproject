#include "presettings.h"
#include "ui_presettings.h"

#include "secondsettingswindow.h"

PreSettings::PreSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreSettings)
{    
    this->s=new SecondSettingsWindow(this);
    ui->setupUi(this);

    setWindowTitle("Settings");


    ui->onOff->addItem("public");
    ui->onOff->addItem("private");

    ui->fromAllComboBox->addItem("receive from all");
    ui->fromAllComboBox->addItem("ask user to confirm before download");

    this->setDefault();

    ui->onOffLabel->setText("public");

    createActions();
    createTrayIcon();

    mSystemTrayIcon->setIcon(QIcon(":/myappicon.png"));
    showIconCheckBox=new QCheckBox(tr("Show options"));
    showIconCheckBox->setChecked(true);
    connect(showIconCheckBox, &QAbstractButton::toggled, mSystemTrayIcon, &QSystemTrayIcon::setVisible);
    mSystemTrayIcon->show();

}

PreSettings::~PreSettings()
{
    delete ui;
}

void PreSettings::setDefault()
{
    string destination=this->s->getDestination();
    this->settings.setDefault(destination);
    if(this->settings.getFromAll()==true)
    {
        ui->fromAllLabel->setText("receive from all");
    }
    ui->destinationLabel->setText(QString::fromUtf8(destination.c_str()));
}

void PreSettings::setVisible(bool visible)
{

        openAction->setEnabled(isMaximized() || !visible);
        QWidget::setVisible(visible);
}

void PreSettings::setDestination(string destination)
{
    this->settings.setDestination(destination);
    ui->destinationLabel->setText(QString::fromUtf8(this->settings.getDestination().c_str()));
}

void PreSettings::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible()) {
            return;
        }
    if (mSystemTrayIcon->isVisible()) {
            /*QMessageBox::information(this, tr("Systray"),
                                     tr("The program will keep running in the "
                                        "system tray. To terminate the program, "
                                        "choose <b>Quit</b> in the context menu "
                                        "of the system tray entry."));*/
            hide();
            event->ignore();
        }
}

void PreSettings::on_onOff_currentIndexChanged(const QString &arg1)
{
    if(arg1=="private")
    {
        this->settings.setOn(false);
    }
    if(arg1=="public")
    {
        this->settings.setOn(true);
    }
    if(this->settings.getOn()==true)
    {
        ui->onOffLabel->setText("public");
    }
    else if(this->settings.getOn()==false)
    {
        ui->onOffLabel->setText("private");
    }
}

void PreSettings::on_settingsButton_clicked()
{
    s->show();
}

void PreSettings::createActions()
{
        openAction = new QAction(tr("&Open settings"), this);
        connect(openAction, &QAction::triggered, this, &QWidget::showNormal);

        quitAction = new QAction(tr("&Quit"), this);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void PreSettings::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(openAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        mSystemTrayIcon = new QSystemTrayIcon(this);
        mSystemTrayIcon->setContextMenu(trayIconMenu);
}

void PreSettings::on_fromAllComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1=="receive from all")
    {
        this->settings.setFromAll(true);
        if(this->settings.getFromAll()==true)
        {
            ui->fromAllLabel->setText("receive from all");
        }
    }
    else if(arg1=="ask user to confirm before download")
    {
        this->settings.setFromAll(false);
        if(!this->settings.getFromAll())
        {
            ui->fromAllLabel->setText("ask user to confirm before download");
        }
    }
}
