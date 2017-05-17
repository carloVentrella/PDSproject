#ifndef PRESETTINGS_H
#define PRESETTINGS_H

#include "secondsettingswindow.h"

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
class PreSettings;
}

class PreSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PreSettings(QWidget *parent = 0);
    ~PreSettings();

    void setDefault(void);

    void setVisible(bool visible) override;

public slots:
    void setDestination(string destination);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_onOff_currentIndexChanged(const QString &arg1);
    void on_settingsButton_clicked();


    void on_fromAllComboBox_currentIndexChanged(const QString &arg1);



private:
    void createActions();
    void createTrayIcon();


    Ui::PreSettings *ui;

    SecondSettingsWindow *s;

    QSystemTrayIcon *mSystemTrayIcon;
    QMenu *trayIconMenu;

    QAction *openAction;
    QAction *quitAction;

    QCheckBox *showIconCheckBox;
    Settings settings;
};

#endif // PRESETTINGS_H
