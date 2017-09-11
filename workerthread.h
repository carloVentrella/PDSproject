#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QObject>
#include <QThread>

#include "user.h"
#include <memory>
class Transfer;


class WorkerThread : public QThread
{
    Q_OBJECT

private:
    Transfer *t;
    int position; //i need to know which bar and label i have to modify
    shared_ptr<User> user; //user related to this thread
    vector<string> files;  //files the thread has to send

public:
    explicit WorkerThread(QObject *parent = 0, Transfer *t=0, int position=0);

    ~WorkerThread();

    void run();

signals:
    void processEvents();
    void progBarModifying(int value, int node);
    void progBarModifying();
    void remTimeModifying(QString value, int node);
    void remTimeModifying(QString value);
    void finished(int node);

public slots:
};

#endif // WORKERTHREAD_H
