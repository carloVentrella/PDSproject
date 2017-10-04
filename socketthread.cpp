#include "socketthread.h"
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QDataStream>

#include <QApplication>

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
    //FORM FOR THE RECEPTION:
    AskConfirmationWindow* askconfirmationwindow=new AskConfirmationWindow("user da mettere", 2, 3, 0);
                                                                            //mettere lo user, il numero di file e la dimensione

    connect(askconfirmationwindow,SIGNAL(canBeReceived()), this, SLOT(reception()));

    //if I don't want to be asked for each reception I don't need the askconfirmationwindow
    if(!Settings::getInstance().getFromAll())
    {
        askconfirmationwindow->show();
    }
    else
    {
        askconfirmationwindow=NULL;
        this->reception();
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

void SocketThread::reception()
{
    //form for the loading wheel
    LoadingWheel* l=new LoadingWheel(0);
    connect(this, SIGNAL(valueChanged(float)), l, SLOT(setProgress(float)));
    l->show();
    qApp->processEvents();

    msleep(500);

    QDataStream in(m_socket);
    fileOpened = false;
    QFile target;
    QString filePath = QString::fromStdString(Settings::getInstance().getDestination());

    qint16 totBytes=m_socket->bytesAvailable();
    while ( (bytesAvailable = m_socket->bytesAvailable()) > 0){

        qDebug()<< "bytesAvailable " << bytesAvailable;
        emit valueChanged((float)(totBytes-bytesAvailable)/totBytes);
        qApp->processEvents();
        msleep(500);


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

               QString uniqueFileName = getUniqueFileName(filePath + "/" + fileName);

               target.setFileName(uniqueFileName);
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

    qDebug()<< "bytesAvailable " << bytesAvailable;

    //to handle the last value of the loading wheel
    if(bytesAvailable==0 && totBytes==0)
        emit valueChanged((float)1);
    else
        emit valueChanged((float)(totBytes-bytesAvailable)/totBytes);

    qApp->processEvents();
    msleep(500);

    l->hide();
    l=nullptr;

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
    while(!f.exists()){
        uniqueFileName = tmp;
        uniqueFileName.append(QString::number(occurrencies++)).append(extesion);
        f.setFileName(uniqueFileName);
    }

    return uniqueFileName;
}
