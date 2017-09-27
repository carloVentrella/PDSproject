#include "workerthread.h"
#include "transfer.h"

#include <QtCore>
#include <iostream>
#include <QHostAddress>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QAbstractSocket>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <memory>
#include <settings.h>

WorkerThread::WorkerThread(QObject *parent, Transfer *t, int position) : QThread(parent)
{
    this->t=t;
    this->position=position;
    this->user=t->getSelected_users().at(position);
    this->files=t->getFiles();


    this->serverAddr = Settings::getInstance().getTCPServerAddr();
    this->serverPort = Settings::getInstance().getTCPServerPort();

}

WorkerThread::~WorkerThread()
{
   this->t=NULL;
   this->user=NULL;
}

void WorkerThread::run()
{
    emit processEvents();

    // TODO remove
    int hr = 100;

    QTcpSocket *socket = new QTcpSocket;
    // when the thread finishes deallocate its objects
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    socket->connectToHost(serverAddr, serverPort);
    socket->waitForConnected();

    if ( (socket->state() != QAbstractSocket::ConnectedState)) {
        qDebug() << "Socket can't connect";
        delete socket;
        return;
    }

    QString curdir;
    QString type;
    QString relativePath;
    quint64 nFiles = files.size();
    quint64 filesSent = 0;
    qint64 written;

    qDebug () << "Ready to send " << nFiles << " items";

    for (std::shared_ptr<QFile> file : files){

        // get fileInfo
        QFileInfo fileInfo(file->fileName());

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        QString tmp="";
        for (QString token : curdir.split("/")){

            if (token.isEmpty())
                continue;

            if (fileInfo.absoluteFilePath().contains(token + "/"))
                tmp.append(token + "/");
            else
                break;
        }
        curdir = tmp;

        if (fileInfo.isDir()){

            type = "DIR";
            curdir.append(fileInfo.baseName().section("/", -1));

            // append (file|dir)name and type
            relativePath = curdir;
            out << (quint32)0 << relativePath << type;

            qDebug() << "Sending dir  [ " << relativePath << " ]";


        }else{

            type = "FILE";

            if (!file->open( QIODevice::ReadOnly )){
                qDebug("Cannot open file, abort");
                delete socket;
                return;
            }

            relativePath = curdir + fileInfo.baseName();
            out << (quint32)0 << relativePath << type;

            qDebug() << "Sending file [ " << relativePath << ", " << file->size() << " ]";

        }

        block.clear();
        out.device()->seek(0);

        // send username
        out << Settings::getInstance().getCurrentUser()->getUsername().c_str();
        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));
        written = socket->write(block);
        socket->flush();

        qDebug() << "Bytes written" << written;

        block.clear();
        out.device()->seek(0);

        // send filename
        out << relativePath.toUtf8();
        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));
        written = socket->write(block);
        socket->flush();

        qDebug() << "Bytes written" << written;

        block.clear();
        out.device()->seek(0);

        // send type
        out << type.toUtf8();
        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));
        written = socket->write(block);
        socket->flush();

        qDebug() << "Bytes written" << written;

        block.clear();
        out.device()->seek(0);

        if (type == "FILE"){

            // send file

            qDebug() << "File size: " << file->size();

            out << (quint32)(file->size());
            written = socket->write(block);
            socket->flush();

            qint64 read = 0;
            written = 0;
            qint64 w;

            while (!file->atEnd() ) {
                QByteArray line = file->read(4096);
                read += line.size();
                w = socket->write(line);

                if (w == -1){
                    qDebug("Cannot write on socket");
                    delete socket;
                    return;
                }

                written +=w;
                socket->flush();

            }

            file->close();
        }

        filesSent++;
        double percentage(filesSent/(double)nFiles*100);

        qDebug() << type << " sent. [" << percentage << "]";

        //modifying the bar of the single user
        emit progBarModifying(percentage,position);

        //modifying the window
        emit processEvents();

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
