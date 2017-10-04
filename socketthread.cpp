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
qint64 totSize = -1;
qint64 totSizeRead = 0;

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
       else if (totSize == -1){

           totSize = QString::fromUtf8(block).toInt();
           qDebug() << "Tot size: " << totSize;

           QString resp("ACC");
           m_socket->write(resp.toUtf8(), resp.length());
           m_socket->flush();
           m_socket->waitForBytesWritten(-1);

           qDebug() << "Response sent [" << resp.length() << "]";

       }
       else if (fileName == ""){
           fileName = QString::fromUtf8(block);   
           qDebug() << "Filename: " << fileName;
       }
       else if (type == ""){
           type = QString::fromUtf8(block);
           qDebug() << "Type: " << type;

           if (type == "FILE")
               fileName = getUniqueFileName(filePath + "/" + fileName);

       }else{

           // it's a file block

           if ( !fileOpened && fileName != "" ){

               target.setFileName(fileName);
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
           totSizeRead += written;

           if (!bytesToRead){
               qDebug("Transfer complete!");
               target.close();
               fileOpened = false;
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

           fileName = "";
           type = "";

       }

       if (!bytesToRead)
           m_blockSize = 0;

       // All files are arrived
       if (totSizeRead==totSize){
           userName = "";
           totSize = -1;
           totSizeRead = 0;
       }

    }

}

QString SocketThread::getUniqueFileName(QString path){

    QFile f(path);
    if (!f.exists()) return path;

    QString uniqueFileName(path);
    int occurrencies = 0;

    QString extesion = "", tmp = "";

    if (uniqueFileName.contains(".")){
        // extract extension
        extesion = QString(".").append(uniqueFileName.right(path.section(".",-1).length()));
        // extract basename without extension
        uniqueFileName = uniqueFileName.left(uniqueFileName.section(".",-1).length()+1);
    }

    tmp = uniqueFileName;

    // loop until a new unique name is find
    while(f.exists()){
        uniqueFileName = tmp;
        uniqueFileName.append(QString::number(occurrencies++)).append(extesion);
        f.setFileName(uniqueFileName);
    }

    return uniqueFileName;
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
