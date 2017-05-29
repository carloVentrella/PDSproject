#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDialog>

#include <vector>
#include <string>
#include <QList>
#include <QListIterator>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <memory>
#include "user.h"
#include "settings.h"

using namespace std;

namespace Ui {
class Transfer;
}

class Transfer : public QDialog
{
    Q_OBJECT

public:
    explicit Transfer(vector<string> files, QWidget *parent = 0);
    ~Transfer();

    QList<shared_ptr<User> > getSelected_users() const;
    void setSelected_users(const QList<shared_ptr<User> > &value);

public slots:
    //transferring files to users
    void transferBegin();

protected:
    //this method has to be rewritten because we don't want to close the application when
    //the window is closed
    void closeEvent(QCloseEvent *event) override;


private:
    Ui::Transfer *ui;

    QList<shared_ptr<User>> selected_users;

    vector<string> files;

    QVBoxLayout *centralLayout;

    QProgressBar *progressBar;

    QLabel *remainingTime;

    QLabel *usersLabel;

    QPushButton *cancelOperation;

    //this flagis useful when we want to stop the operation->
    //nothing will be done after this flag is set
    int flag;

};

#endif // TRANSFER_H
