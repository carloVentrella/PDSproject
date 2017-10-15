#ifndef CHOOSINGDIRECTORYWINDOW_H
#define CHOOSINGDIRECTORYWINDOW_H

#include <QDialog>

#include <QWidget>
#include <QDirModel>
#include <QTreeView>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QDir>


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

    QString getDestination() const;
    void setDestination(const QString &value);

    QString getTmp() const;
    void setTmp(const QString &value);

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
    void destinationChanged(QString destination);

private:
    //download destination dir
    QString destination;

    //temporary directory, to not compromise the real download destination dir
    QString tmp;

    QFileSystemModel *fileSystemModel;

    Ui::ChoosingDirectoryWindow *ui;
};

#endif // CHOOSINGDIRECTORYWINDOW_H
