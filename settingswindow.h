#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

#include "choosingdirectorywindow.h"
#include "choosingiconwindow.h"

#include <QWidget>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QCheckBox>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDialog>
#include <QAbstractButton>
#include <QCoreApplication>


namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

    //this function sets the default value of the application in the window;
    //the default values are in settings.h
    void setDefault(void);

    void setVisible(bool visible) override;

public slots:
    void setDestination(string destination);

    //if the user selects a new icon, settings have to be rewritten
    void setIconChanged(QString icon);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    //this function sets the on/off parameter in settings.h
    void on_onOff_currentIndexChanged(const QString &arg1);


    //this function opens the choosingDirectoryWindow
    void on_settingsButton_clicked();

    //this function sets the fromAll/not parameter in settings.h
    void on_fromAllComboBox_currentIndexChanged(const QString &arg1);


    void on_iconButton_clicked();

private:

    //to create actions that will be performed when the toolbar is used
    void createActions();

    //to create the link between the toolbar and the actions
    void createTrayIcon();

    Ui::SettingsWindow *ui;

    ChoosingDirectoryWindow *s;

    ChoosingIconWindow *i;

    QSystemTrayIcon *mSystemTrayIcon;
    QMenu *trayIconMenu;

    QAction *openAction;
    QAction *quitAction;

    QCheckBox *showIconCheckBox;
};

#endif // SETTINGSWINDOW_H
