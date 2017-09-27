#include "socketthread.h"
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QDataStream>

SocketThread::SocketThread(qintptr descriptor, QObject *parent) : QThread(parent), m_socketDescriptor(descriptor), m_blockSize(0)
{
    // init here
}

SocketThread::~SocketThread()
{
    delete m_socket;
}

void SocketThread::run()
{
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_socketDescriptor);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    exec();
}

qint64 bytesAvailable=0;
qint64 bytesToRead = 0;
qint64 bytesWritten = 0;
bool fileOpened = false;

void SocketThread::onReadyRead()
{
    QDataStream in(m_socket);
    fileOpened = false;
    QFile target;
    QString filePath = "/home/carloventrella/Downloads/dest/";

    while ( (bytesAvailable = m_socket->bytesAvailable()) > 0){

       if ( m_blockSize == 0){
           in >> m_blockSize;
           m_bytesRead = 0;
           bytesToRead = m_blockSize;
       }

       qint64 read = (bytesAvailable < bytesToRead) ? bytesAvailable : bytesToRead;

       if (read < 0){
           qDebug() << "Cannot read " << read << "bytes";
           m_socket->disconnectFromHost();
           return;
       }
       QByteArray block = m_socket->read(read);

       if (!block.size())
           continue;

       bytesToRead -= block.size();

       if (userName == ""){
           userName = QString::fromUtf8(block);
           qDebug() << "Username: " << userName;
       }
       else if (fileName == ""){
           fileName = QString::fromUtf8(block);
           qDebug() << "Filename: " << fileName;
       }
       else if (type == ""){
           type = QString::fromUtf8(block);
            qDebug() << "Type: " << type;

       }else{

           // it's a file block

           if ( !fileOpened && fileName != "" ){

               target.setFileName(filePath + "/" + fileName);
               if (!target.open(QIODevice::WriteOnly | QIODevice::Append)) {
                   qDebug() << "Can't open file for written";
                   m_socket->disconnectFromHost();
                   return;
               }

               if(bytesWritten == 0)
                   target.resize(0);

               fileOpened = true;
           }

           qint64 written = target.write(block,block.size());
           bytesWritten += written;

           if (!bytesToRead){
               qDebug("Transfer complete!");
               target.close();
               fileOpened = false;
               userName = "";
               fileName = "";
               type = "";
           }


       }

       if (type=="DIR"){

           QDir dir(filePath + "/" + fileName);
           if (!dir.exists()) {
               if (!dir.mkpath(".")){
                   qDebug("Cannot create dir");
                   m_socket->disconnectFromHost();
                   return;
               }
           }

           qDebug() << "Dir created.  [ " << fileName << " ]";

           userName = "";
           fileName = "";
           type = "";

       }

       if (!bytesToRead)
           m_blockSize = 0;

    }

}

void SocketThread::onDisconnected()
{
    qDebug("Disconnection");

    m_socket->close();

    // leave event loop
    quit();
}

void SocketThread::onError(QAbstractSocket::SocketError error){

    qDebug() << error;
}
