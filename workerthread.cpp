#include "workerthread.h"

#include <QtCore>
#include "transfer.h"
#include <iostream>

WorkerThread::WorkerThread(QObject *parent, Transfer *t, int position) : QThread(parent)
{
    this->t=t;
    this->position=position;
    this->user=t->getSelected_users().at(position);
    this->files=t->getFiles();

}

WorkerThread::~WorkerThread()
{
   this->t=NULL;
   this->user=NULL;
}

void WorkerThread::run()
{
    int hr=100;  //DA TOGLIERE, MI SERVIVA SOLO PER IL CICLO FOR

    emit processEvents();

   for(int i=0;i<=100 && this->t->getFlagAtNode(position)==0; i++)
        {

            cout<<std::to_string(i)<<endl;

            //to simulate delay of transmission
            this->msleep((this->position+1)*200);

            //modifying the bar of the single user
            emit progBarModifying(i,position);

            //modifying the window
            emit processEvents();

            hr=100-i;

            //modifying the remaining time for the single user
            emit remTimeModifying(QString::fromStdString(std::to_string(hr).append(" seconds left").c_str()), position);

            //modifying the general progress bar
            emit progBarModifying();

            emit processEvents();

            //modifying the general remaining time
            emit remTimeModifying(QString::fromStdString(to_string(100-this->t->getProgressBar()->value()).append(" seconds left").c_str()));

            emit processEvents();

        }


    //handle the stop of the transfer in a clean way

    emit finished(position);
}
