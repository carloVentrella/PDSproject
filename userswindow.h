#ifndef USERSWINDOW_H
#define USERSWINDOW_H

#include <QDialog>

#include "users.h"

#include <QCheckBox>
#include <QGridLayout>
#include <map>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include <QList>
#include <QListIterator>
#include <QCloseEvent>

using namespace std;

namespace Ui {
class UsersWindow;
}

class UsersWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UsersWindow(shared_ptr<Users> users, QWidget *parent = 0);
    ~UsersWindow();

protected:
    //this method has to be rewritten because we don't want to close the application when
    //the window is closed
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::UsersWindow *ui;

    //this contains the map of users that has to be shown in the window
    shared_ptr<Users> u;

    //parameters of the gridLayout
    const int NUM_COL=4;
    int NUM_ROWS;

    //this function creates the label associated with the icon
    QLabel* createPixMapLabels(QIcon p);

    //this is the layout for the window that contains labels, checkbox
    //and the final button
    QGridLayout *centralLayout;

    //this is the list of checkbox
    QList<QCheckBox *> allButtons;

};

#endif // USERSWINDOW_H
