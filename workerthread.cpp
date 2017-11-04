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
#include <QDateTime>

QHostAddress WorkerThread::getServerAddr() const
{
    return serverAddr;
}

void WorkerThread::setServerAddr(const QHostAddress &value)
{
    serverAddr = value;
}

WorkerThread::WorkerThread(QObject *parent, Transfer *t, int position) : QThread(parent)
{
    this->t=t;
    this->position=position;
    this->user=t->getSelected_users().at(position);
    this->files=t->getFiles();
    this->totSize = 0;
    this->totSizeWritten = 0;

    // sum size of files
    for ( shared_ptr<QFile> f : this->files){
        QFileInfo qf(f->fileName());

        // skip folder
        if (qf.isDir())
            continue;

        qDebug() << f->fileName() << " -> " << f->size();
        this->totSize += f->size();
    }

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

    QTcpSocket *socket = new QTcpSocket;
    // when the thread finishes deallocate its objects
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    qDebug() << "Connecting to " << serverAddr << "," << serverPort;

    socket->connectToHost(serverAddr, serverPort);
    socket->waitForConnected();

    if ( (socket->state() != QAbstractSocket::ConnectedState)) {
        qDebug() << "Socket can't connect";
        delete socket;

        emit errorHandling(position, "Socket can't connect"); 
        updateProgresses(position, 100, 0, 100-this->t->getProgressBar()->value());
        //handle the stop of the transfer in a clean way
        emit finished(position);

        return;
    }    

    QString curdir;
    QString type;
    QString relativePath;
    quint64 nFiles = files.size();
    quint64 filesSent = 0;
    qint64 written;

    qDebug () << "Ready to send " << nFiles << " items";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();
    out.device()->seek(0);

    // send username
    out << Settings::getInstance().getCurrentUser()->getUsername().toUtf8();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    written = socket->write(block);
    socket->flush();

    qDebug() << Settings::getInstance().getCurrentUser()->getUsername() << " written" << written;

    block.clear();
    out.device()->seek(0);

    qDebug() << "Tot size: " << this->totSize;

    // send totsize
    out << QString::number(this->totSize).toUtf8();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    written = socket->write(block);
    socket->flush();

    qDebug() << "Bytes written" << written;

    socket->waitForReadyRead(-1);

    // Read ACC or REF
    QByteArray block_rd = socket->readAll();
    QString response = QString::fromUtf8(block_rd);
    qDebug() << "Response: " << response;

    if (response == "REF")
    {
        emit errorHandling(position, "REFUSED");
        // TODO fix wrong parameter
        updateProgresses(position, 100, 0, 100-this->t->getProgressBar()->value());
        //handle the stop of the transfer in a clean way
        emit finished(position);

        return;
    }

    for (std::shared_ptr<QFile> file : files){

        int startTime = QDateTime::currentSecsSinceEpoch();

        // get fileInfo
        QFileInfo fileInfo(file->fileName());

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

                emit errorHandling(position, "Cannot open file, abort");                
                // TODO fix wrong parameter
                updateProgresses(position, 100, 0, 100-this->t->getProgressBar()->value());
                //handle the stop of the transfer in a clean way
                emit finished(position);

                return;
            }
            relativePath = curdir + fileInfo.fileName();
            out << (quint32)0 << relativePath << type;

            qDebug() << "Sending file [ " << relativePath << ", " << file->size() << " ]";

        }

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

                qDebug() << w << " bytes sent";

                if (w == -1){
                    qDebug("Cannot write on socket");
                    delete socket;

                    emit errorHandling(position, "Cannot write on socket");

                    // TODO fix wrong parameter
                    updateProgresses(position, 100, 0, 100);
                    //handle the stop of the transfer in a clean way
                    emit finished(position);

                    return;
                }

                this->totSizeWritten +=w;
                written +=w;


                int duration = QDateTime::currentSecsSinceEpoch() - startTime;
                qDebug() << "Time elapsed: " << duration << "s";
                float speed = this->totSizeWritten / (float)(duration);
                qDebug() << "Speed: " << speed << "b/s";

                int globalEstimate = (this->totSize - this->totSizeWritten)/speed;
                int localEstimate = (file->size() - written) /speed;

                qDebug() << "remaining time: " << globalEstimate << "s";

                double percentage(this->totSizeWritten/(double)this->totSize*100);

                updateProgresses(position, percentage, localEstimate, globalEstimate);

                socket->flush();
                socket->waitForBytesWritten();

            }

            file->close();
        }

        filesSent++;

    }

    //handle the stop of the transfer in a clean way
    qDebug() << "All files sent";

    // Wait client to finish
    socket->waitForReadyRead(-1);
    QByteArray block_final = socket->readAll();
    QString result = QString::fromUtf8(block_rd);
    qDebug() << "Response: " << result;

    emit finished(position);
}

void WorkerThread::updateProgresses(int position, int percentage, int userRemtime, int globalRemtime){

    emit progBarModifying(percentage, position);

    //modifying the remaining time for the single user
    emit remTimeModifying(userRemtime, position);

    //modifying the general progress bar
    emit progBarModifying();

    emit processEvents();

}

