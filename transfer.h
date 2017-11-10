#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDialog>

#include <vector>
#include <string>
#include <QFile>
#include <QList>
#include <QListIterator>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <memory>
#include "user.h"
#include "settings.h"
#include <QPalette>
#include <QThread>
#include "workerthread.h"

#include <QMutex>

using namespace std;

namespace Ui {
class Transfer;
}

class Transfer : public QDialog
{
    Q_OBJECT

public:
    explicit Transfer(QList<shared_ptr<User>> selected_users,QList<std::shared_ptr<QFile>> files, QWidget *parent = 0);
    ~Transfer();

    //getter and setter for the users list
    QList<shared_ptr<User>> getSelected_users() const;
    void setSelected_users(const QList<shared_ptr<User> > &value);

    //this function has to be done after the terination of a thread, if it terminates before work is over
    void handleCancelation(int node);

    //getter for the flag of a specific node
    int getFlagAtNode(int node);

    //getter and setter for the general progBar
    QProgressBar *getProgressBar() const;
    void setProgressBar(QProgressBar *value);

    //getter and setter for the label of the general remainingTime
    QLabel *getRemainingTime() const;
    void setRemainingTime(QLabel *value);

    //getter for progBar and remainingTime for each user
    QProgressBar * getProgressBarPerSingleTransfer(int node)
    {
        return this->progressBarPerSingleTransfer.at(node);
    }

    QLabel* getRemainingTimePerSingle(int node){
        return this->remainingTimePerSingleTransfer.at(node);
    }

    //getter and setter for the vector of files
    QList<std::shared_ptr<QFile>> getFiles() const;
    void setFiles(const QList<std::shared_ptr<QFile>> &value);

public slots:
    //transferring files to users
    void transferBegin();

    //handling the end of the transfer
    void transferEnd(int node);

    //handle the update of the window
    void handleProcessEvents();

    void handleProgBarModifying(int value, int node);
    void handleProgBarModifying();

    void handleRemTimeModifying(int value, int node);
    void handleRemTimeModifying();


    void errorsHandler(int node, QString error);

protected:
    //this method has to be rewritten because we don't want to close the application when
    //the window is closed
    void closeEvent(QCloseEvent *event) override;


private:
    Ui::Transfer *ui;

    QList<shared_ptr<User>> selected_users;

    QList< std::shared_ptr<QFile>> files;

    QVBoxLayout *verticalLayout;

    //button to show the widgets for each user
    QPushButton *singleTransfers;

    QProgressBar *progressBar;
    QLabel *remainingTime;
    QLabel *usersLabel;
    QPushButton *cancelOperation;

    //widget per single transfer
    QList<QProgressBar*> progressBarPerSingleTransfer;
    QList<QLabel*> remainingTimePerSingleTransfer;
    QList<QLabel*> usersLabelPerSingleTransfer;
    QList<QPushButton*> cancelOperationPerSingleTransfer;

    //this vector of flags is used to understand whether a thread has terminated or not
    std::vector<int> flags;

    //boolean to handle if widgets for each user are visible or not
    bool visible;

    //this flag will determine if the window has to be closed (hidden)
    int flagToQuit;

    //listof threads for each sending process
    QList<WorkerThread*> sendingThreads;

    QMutex mBar;
    QMutex mTime;

    // global remaining time
    QList<int> remtimeValuePerSingleTransfer;
    int maxRemTime=0;
};

#endif // TRANSFER_H
