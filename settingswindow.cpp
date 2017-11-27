#include "settingswindow.h"
#include "ui_settingswindow.h"


SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    this->s=new ChoosingDirectoryWindow(this);
    this->i=new ChoosingIconWindow(this);

    ui->setupUi(this);

    setWindowTitle("Settings");

    //to verify what the default values are, so they can be showed as well
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

    //this signal is emitted only when the user effectively change the icon
    connect(this->i, SIGNAL(iconChanged(QString)), this, SLOT(setIconChanged(QString)));

    createActions();
    createTrayIcon();

    mSystemTrayIcon->setIcon(QIcon(":/myappicon.png"));
    showIconCheckBox=new QCheckBox(tr("Show options"));
    showIconCheckBox->setChecked(true);
    connect(showIconCheckBox, &QAbstractButton::toggled, mSystemTrayIcon, &QSystemTrayIcon::setVisible);
    mSystemTrayIcon->show();

}

SettingsWindow::~SettingsWindow()
{
    delete s;
    delete i;
    delete showIconCheckBox;
    delete trayIconMenu;
    delete mSystemTrayIcon;
    delete openAction;
    delete quitAction;
    delete ui;
}


void SettingsWindow::setDefault()
{
    QString destination=this->s->getDestination();
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

    ui->destinationLabel->setText(destination);

    //initial label of the user for the icon
    QString icon=Settings::getInstance().getThumbPath();
    icon=icon.mid(13,icon.length()-13-4);
    ui->iconLabel->setText(icon);

}

void SettingsWindow::setVisible(bool visible)
{
        openAction->setEnabled(isMaximized() || !visible);
        QWidget::setVisible(visible);
}

void SettingsWindow::setDestination(QString destination)
{
    Settings::getInstance().setDestination(destination);
    ui->destinationLabel->setText(Settings::getInstance().getDestination());
}

void SettingsWindow::setIconChanged(QString icon)
{
    Settings::getInstance().setThumbPath(icon);
    icon=icon.mid(13,icon.length()-13-4);
    ui->iconLabel->setText(icon);
}

void SettingsWindow::closeEvent(QCloseEvent *event)
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

void SettingsWindow::on_onOff_currentIndexChanged(const QString &arg1)
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

void SettingsWindow::on_settingsButton_clicked()
{
    s->show();
}

void SettingsWindow::createActions()
{
        openAction = new QAction(tr("&Open settings"), this);
        connect(openAction, &QAction::triggered, this, &QWidget::showNormal);

        quitAction = new QAction(tr("&Quit"), this);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SettingsWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(openAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);

        mSystemTrayIcon = new QSystemTrayIcon(this);
        mSystemTrayIcon->setContextMenu(trayIconMenu);
}

void SettingsWindow::on_fromAllComboBox_currentIndexChanged(const QString &arg1)
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

void SettingsWindow::on_iconButton_clicked()
{
    this->i->show();
}
