#ifndef CHOOSINGDIRECTORYWINDOW_H
#define CHOOSINGDIRECTORYWINDOW_H

#include <QDialog>

#include <QWidget>
#include <QDirModel>
#include <QTreeView>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QDir>
#include <string>

#include "settings.h"

namespace Ui {
class ChoosingDirectoryWindow;
}

class ChoosingDirectoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChoosingDirectoryWindow(QWidget *parent = 0);
    ~ChoosingDirectoryWindow();

    std::string getDestination() const;
    void setDestination(const std::string &value);

    std::string getTmp() const;
    void setTmp(const std::string &value);

private slots:

    //this method is called when someone double-clicks the treeView
    void on_tree_doubleClicked(const QModelIndex &index);

    //this method is called when someone pushes the "Apply" button
    void on_applyButton_clicked();

    //this method is called when someone pushes the "Cancel" button
    void on_cancelButton_clicked();

signals:
    //this method has to be called when the "Apply" button is pushed and we have to
    //emit a signal to the SettingsWindow
    void destinationChanged(string destination);

private:
    //download destination dir
    std::string destination;

    //temporary directory, to not compromise the real download destination dir
    std::string tmp;

    QFileSystemModel *fileSystemModel;

    Ui::ChoosingDirectoryWindow *ui;
};

#endif // CHOOSINGDIRECTORYWINDOW_H
