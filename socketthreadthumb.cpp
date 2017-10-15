#include "socketthreadthumb.h"

#include <QByteArray>
#include <QDataStream>
#include <QPixmap>
#include <QIcon>
#include <QHostAddress>

#include <QFile>
#include <QBuffer>

SocketThreadThumb::SocketThreadThumb(qintptr descriptor, shared_ptr<Users> users,  QObject *parent) : QThread(parent), m_socketDescriptor(descriptor), m_blockSize(0), users(users)
{
    // init here
}

SocketThreadThumb::~SocketThreadThumb()
{
    delete m_socket;
}

void SocketThreadThumb::run()
{
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_socketDescriptor);

    usr = users->getUser(m_socket->peerAddress().toString());

    if (usr == nullptr){
        qDebug() << "Cannot receive thumb, usr not in user list";
        QThread::quit();
    }

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    exec();
}

void SocketThreadThumb::onDisconnected()
{
    qDebug("Disconnection");

    m_socket->close();

    // leave event loop
    quit();
}

void SocketThreadThumb::onError(QAbstractSocket::SocketError error){

    qDebug() << error;
    QThread::quit();
}

// bytes available in the socket
qint64 bAvailable=0;
qint64 bToRead = 0;
QByteArray encoded;

void SocketThreadThumb::onReadyRead()
{
    // Receive thumb

    QDataStream in(m_socket);

    while ( (bAvailable = m_socket->bytesAvailable()) > 0){

        if ( m_blockSize == 0){

            in >> m_blockSize;
            m_bytesRead = 0;
            bToRead = m_blockSize;
            encoded.clear();
            qDebug() << "Receiving thumb size [" << bToRead <<"]";

        }

        qint64 read = (bAvailable < bToRead) ? bAvailable : bToRead;

        if (read < 0){
            qDebug() << "Cannot read " << read << "bytes";
            m_socket->disconnectFromHost();
            return;
        }

        QByteArray block = m_socket->read(read);
        // update the number of bytes to read
        bToRead -= block.size();

        if (!block.size())
            continue;

        encoded.append(block);

        if (!bToRead){

            m_blockSize = 0;

            QPixmap p;
            QIcon thumb;

            QBuffer buffer;
            buffer.open(QIODevice::WriteOnly);
            qint64 bytes = buffer.write(encoded); // Read all the received bytes
            buffer.seek(buffer.pos() - bytes);  // go back as many bytes as we just wrote so that it can be read

            qDebug() << "Read [" << bytes << "]";
            qDebug() << "Buffer size [" << buffer.buffer().size() << "]";
            qDebug() << buffer.buffer();
            QByteArray a = QByteArray::fromBase64(buffer.buffer());

            p.loadFromData(a, "PNG");
            if (p.isNull()){
                qDebug("The image is null. Something failed.");
                m_socket->disconnectFromHost();
                QThread::quit();
            }
            thumb.addPixmap(p);

            qDebug() << "Thumb received";

            usr->setThumbnail(thumb);

            m_socket->disconnectFromHost();

            QThread::quit();
        }
    }

}
