#include "thumbsender.h"
#include "user.h"
#include "settings.h"

#include <QTcpSocket>
#include <QBuffer>

ThumbSender::ThumbSender(QHostAddress address, qint16 p, shared_ptr<Users> users,  QObject *parent) : QObject(parent), addr(address), port (p), users(users)
{

}


ThumbSender::~ThumbSender() {
}

void ThumbSender::process() {


    qDebug() << "Sending thumb to user with addr " << addr.toString();
    shared_ptr<User> usr(users->getUser(addr.toString()));

    QTcpSocket *socket = new QTcpSocket;
    // when the thread finishes deallocate its objects
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

    qDebug() << "Sending thumb to " << usr->getUsername();
    qDebug() << "Connecting to " << addr << "," << port;

    socket->connectToHost(addr, port);
    socket->waitForConnected();

    if ( (socket->state() != QAbstractSocket::ConnectedState)) {
        qDebug() << "Socket can't connect";
        delete socket;
        emit finished();
        return;
    }

    qDebug() << "Connection ok";

    shared_ptr<User> curUsr(Settings::getInstance().getCurrentUser());

    QIcon icn = curUsr->getThumbnail();
    QPixmap pixmap = icn.pixmap(icn.actualSize(QSize(100,100)));

    QByteArray ba,bb;              // Construct a QByteArray object
    QDataStream out(&ba, QIODevice::WriteOnly);
    ba.clear();


    // out.device()->seek(0);

    QBuffer buffer(&bb);        // Construct a QBuffer object using the QbyteArray
    buffer.open(QIODevice::WriteOnly);
    out.device()->seek(4);
    if (!pixmap.save(&buffer, "PNG")){
        qDebug("Cannot send thumb");
        socket->disconnectFromHost();
        emit finished();
        return;
    }

    bb=buffer.data().toBase64();

    qDebug() << "sssize: " << ba.size();

    out.device()->seek(0);

    out << (quint32)(bb.size());
    ba.append(bb);
    qint64 written = socket->write(ba);          // Send the QBuffer (QbyteArray) over a socket
    socket->flush();
    qDebug() << "Thumb size ["<< bb.size()<<"]";
    qDebug() << "Thumb sent ["<< written<<"]";

    socket->disconnectFromHost();

    emit finished();

}
