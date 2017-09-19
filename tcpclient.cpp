#include "tcpclient.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QAbstractSocket>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <memory>


void Client::createFileList(QFileInfoList FileInfoList){

    for (QFileInfo fileInfo : FileInfoList){

        if (fileInfo.fileName() == "." | fileInfo.fileName() == "..")
            continue;

        // qDebug() << "Filename : " << fileInfo.absoluteFilePath() ;
        // qDebug() << "   Exists? " << fileInfo.exists();
        // qDebug() << "   Is dir?" << fileInfo.isDir();
        // qDebug() << "   Is symlink? " << fileInfo.isSymLink();
        // qDebug() << "   Read-only? " << fileInfo.isReadable();

        if (!fileInfo.exists() | !fileInfo.isReadable())
            continue;

        fileList.push_back( std::shared_ptr<QFile>( new QFile(fileInfo.absoluteFilePath())) );

        if (fileInfo.isDir()){
            // Dir:  add it to the list and recur on it
            QDir dir(fileInfo.absoluteFilePath());
            createFileList(dir.entryInfoList());
        }

    }

}

void Client::send()
{
    QDir dirtest("/home/carloventrella/testTCP/folderTest/nesterFolder");
    QString path = dirtest.absolutePath();

    QFileInfoList files;
    QFileInfo fileInfo(path);
    files.append(fileInfo);
    createFileList(files);

    if( fileList.size() == 0){
        qDebug("No file to send.");
        return;
    }

    QTcpSocket *socket = new QTcpSocket;
    // when the thread finishes deallocate its objects
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    socket->connectToHost(m_host, m_port);
    socket->waitForConnected();

    if ( (socket->state() != QAbstractSocket::ConnectedState)) {
        qDebug() << "Socket can't connect";
        delete socket;
        return;
    }

    QString curdir;
    QString type;
    QString relativePath;
    quint64 nFiles = fileList.size();
    quint64 filesSent = 0;
    qint64 written;

    qDebug () << "Ready to send " << nFiles << " items";

    for (std::shared_ptr<QFile> file : fileList){

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
        qDebug() << type << " sent. [" << filesSent/(double)nFiles*100 << "]";

    }

    return;


}
