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

    bool on = Settings::getInstance().getOn();
    bool fromAll=Settings::getInstance().getFromAll();

    ui->onOff->addItem("public");
    ui->onOff->addItem("private");

    if(!on)
    {
        ui->onOff->setCurrentIndex(1);
    }

    ui->fromAllComboBox->addItem("receive from all");
    ui->fromAllComboBox->addItem("ask user to confirm before download");

    if(!fromAll)
    {
        ui->fromAllComboBox->setCurrentIndex(1);
    }

    this->setDefault();

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
    Settings::getInstance().setDestination(destination);

    if(Settings::getInstance().getFromAll()==1)
    {
        ui->fromAllLabel->setText("receive from all");
    }
    else
    {
        ui->fromAllLabel->setText("ask user to confirm before download");
    }

    if(Settings::getInstance().getOn())
    {
        ui->onOffLabel->setText("public");
    }
    else
    {
        ui->onOffLabel->setText("private");
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
    Settings::getInstance().setDestination(destination);
    ui->destinationLabel->setText(QString::fromUtf8(Settings::getInstance().getDestination().c_str()));
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
        Settings::getInstance().setOn(false);
    }
    if(arg1=="public")
    {
        Settings::getInstance().setOn(true);
    }
    if(Settings::getInstance().getOn()==true)
    {
        ui->onOffLabel->setText("public");
    }
    else if(Settings::getInstance().getOn()==false)
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
        Settings::getInstance().setFromAll(true);
        if(Settings::getInstance().getFromAll()==true)
        {
            ui->fromAllLabel->setText("receive from all");
        }
    }
    else if(arg1=="ask user to confirm before download")
    {
        Settings::getInstance().setFromAll(false);
        if(!Settings::getInstance().getFromAll())
        {
            ui->fromAllLabel->setText("ask user to confirm before download");
        }
    }
}
