#ifndef SECONDSETTINGSWINDOW_H
#define SECONDSETTINGSWINDOW_H

#include <QDialog>

#include <QWidget>
#include<QDirModel>
#include <QTreeView>
#include <QModelIndex>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QDir>
#include <string>


#include "settings.h"

namespace Ui {
class SecondSettingsWindow;
}

class SecondSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondSettingsWindow(QWidget *parent = 0);
    ~SecondSettingsWindow();

    void setDefaultS();

    std::string getDestination() const;
    void setDestination(const std::string &value);

    std::string getTmp() const;
    void setTmp(const std::string &value);

private slots:

    void on_tree_doubleClicked(const QModelIndex &index);


    void on_applyButton_clicked();

    void on_cancelButton_clicked();

signals:
    void destinationChanged(string destination);

private:
    Ui::SecondSettingsWindow *ui;
    std::string destination;
    std::string tmp;
    QFileSystemModel *fileSystemModel;
};

#endif // SECONDSETTINGSWINDOW_H
