#ifndef USERSWINDOW_H
#define USERSWINDOW_H

#include <QDialog>

#include "users.h"
#include "transfer.h"
#include "discovery.h"

#include <QCheckBox>
#include <QGridLayout>
#include <map>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include <QList>
#include <QListIterator>
#include <QCloseEvent>
#include <QMessageBox>

using namespace std;

namespace Ui {
class UsersWindow;
}

class UsersWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UsersWindow(shared_ptr<discovery> scout, QList<std::shared_ptr<QFile>>, shared_ptr<Users> users, QWidget *parent = 0);
    ~UsersWindow();

protected:
    //this method has to be rewritten because we don't want to close the application when
    //the window is closed
    void closeEvent(QCloseEvent *event) override;

public slots:
    //whichUser: which user has been removed or added
    //state: true->added
    //       false->removed
    void handleNewOrRemovedUsers(string whatsNeeded, int whichUser, bool state);

    void handleModThumb(const QIcon &value, string username);

signals:
    //this function begins the trasfer
    void startTransfer();

private:
    Ui::UsersWindow *ui;

    //this function creates the label associated with the icon
    QLabel* createPixMapLabels(QIcon p);

    //this contains the map of users that has to be shown in the window
    shared_ptr<Users> u;

    //vector of selected files
    QList< std::shared_ptr<QFile>> files;

    //this is necessary to find the IP from the username
    QMap<string,string> usersMap;

    //list of users to be returned
    QList<shared_ptr<User>> selected_users;

    //parameters of the gridLayout
    const int NUM_COL=4;
    int NUM_ROWS;

    //parameters of the last element added
    int x_lastElement;
    int y_lastElement;

    QList<QVBoxLayout*> allLayouts;

    //this is the layout for the window that contains labels, checkbox
    //and the final button
    QGridLayout *centralLayout;

    //this is the list of checkbox
    QList<QCheckBox *> allButtons;

    //this is the list of labels with icons
    QList<QLabel*> icons;

    //button to share
    QPushButton *buttonToShare;
    QVBoxLayout *buttonLayout;

    //this scout will be able to handle when informations about users change
    shared_ptr<discovery> scout;
};

#endif // USERSWINDOW_H
