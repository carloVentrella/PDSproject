#include "socketthread.h"
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QDataStream>
#include <QApplication>

#include  <QMessageBox>

SocketThread::SocketThread(qintptr descriptor, QObject *parent) : QThread(parent), m_socketDescriptor(descriptor), m_blockSize(0)
{
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

    // If the username is not set there's no transfer in progress
    this->userName = "";

    exec();
}

// bytes available in the socket
qint64 bytesAvailable=0;
qint64 bytesToRead = 0;
// bytes written in the target file
qint64 bytesWritten = 0;
bool fileOpened = false;
// amount of bytes to read
qint64 totSize = -1;
// amount of bytes read
qint64 totSizeRead = 0;
qint64 fileRead = 0;
LoadingWheel* l = nullptr;
// lookup table of unique names
std::map<QString,QString> uniqueDirNames;


void SocketThread::onDisconnected()
{
    qDebug("Disconnection");

    m_socket->close();


    if(l!=nullptr)
    {
        //QMessageBox::warning(0, "Warning", "..Disconnection..");
        // hide the loading wheel
        emit valueChanged((float)1);
        l->hide();
        l=nullptr;
    }

    // leave event loop
    quit();
}

void SocketThread::onError(QAbstractSocket::SocketError error){

    qDebug() << error;


    if(l!=nullptr)
    {
        switch (error) {
             case QAbstractSocket::RemoteHostClosedError:
                 QMessageBox::warning(0, tr("Error"),
                                     tr("The host has disconnected."));
            break;
             case QAbstractSocket::HostNotFoundError:
                 QMessageBox::warning(0, tr("Error"),
                                          tr("The host was not found. Please check the "
                                             "host name and port settings."));
                 break;
             case QAbstractSocket::ConnectionRefusedError:
                 QMessageBox::warning(0, tr("Error"),
                                          tr("The connection was refused by the peer. "
                                             "Make sure the fortune server is running, "
                                             "and check that the host name and port "
                                             "settings are correct."));
                 break;
             default:
                 QMessageBox::warning(0, tr("Error"),
                                          tr("..Network Error.."));
             }

        // hide the loading wheel
        emit valueChanged((float)1);
        l->hide();
        l=nullptr;
    }
}


void SocketThread::onReadyRead()
{

    QDataStream in(m_socket);
    fileOpened = false;
    QFile target;
    QString filePath = Settings::getInstance().getDestination();

    while ( (bytesAvailable = m_socket->bytesAvailable()) > 0){

       // If all available bytes were comsumed the new packet
       // will be a block size
       if ( m_blockSize == 0){
           in >> m_blockSize; 
           m_bytesRead = 0;
           bytesToRead = m_blockSize;
       }

       // Compute the number of bytes to read
       qint64 read = (bytesAvailable < bytesToRead) ? bytesAvailable : bytesToRead;

       if (read < 0){
           qDebug() << "Cannot read " << read << "bytes";
           m_socket->disconnectFromHost();

           if(l!=nullptr)
           {
               QMessageBox::warning(0, "Warning",  "Cannot read a negative quantity of bytes");
               // hide the loading wheel
               emit valueChanged((float)1);
               l->hide();
               l=nullptr;
           }

           return;
       }

       // Read the block

       // It can contain:
       // - username
       // - amount of bytes to read
       // - filename
       // - type (FILE|DIR)
       // - block of file

       QByteArray block = m_socket->read(read);

       if (!block.size())
           continue;

       // update the number of bytes to read
       bytesToRead -= block.size();

       // Set the username
       if (userName == ""){
           userName = block;
           qDebug() << "Username: " << userName;
       }
       // set the amount of bytes to read
       else if (totSize == -1){

           totSize = block.toInt();
           qDebug() << "Tot size: " << totSize;

           if(!Settings::getInstance().getFromAll())
           {
               // if the property getFromAll is false
               // prompt a dialog to let the user decide
               AskConfirmationWindow* askconfirmationwindow=new AskConfirmationWindow(this->userName, totSize);                                                                        //mettere lo user, il numero di file e la dimensione
               connect(askconfirmationwindow,SIGNAL(canBeReceived()), this, SLOT(sendConfirmationAccept()));
               connect(askconfirmationwindow,SIGNAL(cannotBeReceived()), this, SLOT(sendConfirmationRefuse()));
               askconfirmationwindow->show();
           }else{

               // Accept the transfer without prompting a dialog
               this->sendConfirmationAccept();
           }

       }
       // set the filename
       else if (fileName == ""){

           fileName = QString::fromUtf8(block);   
           qDebug() << "Filename: " << fileName;

           // if this is the first file start the loading wheel
           if (fileRead == 0 && l == nullptr){

               // start loading wheel
               l=new LoadingWheel();
               connect(this, SIGNAL(valueChanged(float)), l, SLOT(setProgress(float)));
               l->show();
               qApp->processEvents();
           }

       }
       // set the type of the file
       else if (type == ""){
           type = QString::fromUtf8(block);
           qDebug() << "Type: " << type;

           if (type == "FILE"){
               fileName = getUniqueFileName(filePath,fileName, true);
           }
       }else{

           // it's a file block
           // add it to the target file

           // open in write-only mode the target file only once
           if ( !fileOpened && fileName != "" ){

               target.setFileName(fileName);
               if (!target.open(QIODevice::WriteOnly | QIODevice::Append)) {
                   qDebug() << "Can't open file for writing";
                   m_socket->disconnectFromHost();

                   if(l!=nullptr)
                   {
                       QMessageBox::warning(0, "Warning",  "Can't open file for writing");
                       // hide the loading wheel
                       emit valueChanged((float)1);
                       l->hide();
                       l=nullptr;
                   }

                   return;
               }

               if(bytesWritten == 0)
                   target.resize(0);

               fileOpened = true;
           }

           // append the block to the target file
           // and update the number of bytes written
           qint64 written = target.write(block,block.size());
           bytesWritten += written;
           totSizeRead += written;

           // update loading wheel
           emit valueChanged((float)(totSizeRead)/totSize);
           qApp->processEvents();

           // if there are no bytes left to read
           // the transfer is finished

           if (!bytesToRead){
               target.close();
               fileOpened = false;
               fileName = "";
               type = "";

               // update number of files completed
               fileRead++;
           }

       }

       if (type=="DIR"){

           // if type is dir just create it,
           // if it does not exists
           QDir dir(getUniqueFileName(filePath, fileName, false));
           if (!dir.exists()) {
               if (!dir.mkpath(".")){
                   qDebug("Cannot create dir");
                   m_socket->disconnectFromHost();


                   if(l!=nullptr)
                   {
                       QMessageBox::warning(0, "Warning",  "Cannot create dir");
                       // hide the loading wheel
                       emit valueChanged((float)1);
                       l->hide();
                       l=nullptr;
                   }

                   return;
               }
           }

           qDebug() << "Dir created.  [ " << filePath + "/" + fileName << " ]";

           fileName = "";
           type = "";

       }

       if (!bytesToRead)
           m_blockSize = 0;

       // All files are arrived
       if (totSizeRead==totSize){

           qDebug() << "Transfer complete";

           userName = "";
           totSize = -1;
           totSizeRead = 0;
           fileRead=0;
           uniqueDirNames.clear();

           this->sendConfirmationResponse("OK");

           msleep(1000);
           // hide the loading wheel
           emit valueChanged((float)1);
           l->hide();
           l=nullptr;
       }

    }
}

void SocketThread::sendConfirmationAccept(){ 
    this->sendConfirmationResponse( QString("ACC") );
}
void SocketThread::sendConfirmationRefuse(){
    this->sendConfirmationResponse( QString("REF") );
}

void SocketThread::sendConfirmationResponse(QString resp){

    m_socket->write(resp.toUtf8(), resp.length());
    m_socket->flush();
    m_socket->waitForBytesWritten(-1);

}

QString SocketThread::getUniqueFileName(QString basepath, QString filename, bool isFile){

    if (basepath.endsWith("/"))
        basepath=basepath.mid(0,basepath.length()-1);

    if (filename.startsWith("/"))
        filename=filename.mid(1);


    qDebug() << "Original path: " << basepath + "/" + filename;

    int occurrencies = 0;
    QString tmp = "",
            left = "",
            uniqueToken(filename),
            token(filename);

    if (filename.contains("/")){
        int firtslash = filename.indexOf("/");
        token = filename.left(firtslash);
        left = filename.mid(firtslash+1);
    }
    else{
        if (filename.contains(".")){
            token = filename.left( filename.lastIndexOf(".") );
            left = QString(".").append(filename.right(filename.section(".",-1).length()));
        }
    }

    uniqueToken = token;

    if (uniqueDirNames.find(token) != uniqueDirNames.end()){
        uniqueToken = uniqueDirNames.at(token);
    }else{
        QString ext = left.startsWith(".") ? left : "";
        QFile f(basepath + "/" + token + ext);
        // loop until a new unique name is find
        while(f.exists()){
            uniqueToken = token;
            uniqueToken.append(QString::number(occurrencies++));
            f.setFileName(basepath + "/" + uniqueToken);
        }

        uniqueDirNames.insert(std::pair<QString,QString>(token,uniqueToken));
    }

    QString sl = (left.startsWith(".") || isFile) ? "" : "/";
    qDebug() << "Unique filename: " << basepath + "/" + uniqueToken + sl + left;
    return basepath + "/" +  uniqueToken + sl + left;
}
