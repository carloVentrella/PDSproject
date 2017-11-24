#include "tcpthumbserver.h"
#include "socketthreadthumb.h"

ThumbServer::ThumbServer(QHostAddress host, quint16 port, shared_ptr<Users> users, QObject *parent)
    : QTcpServer(parent),
      m_host(host),
      m_port(port),
      users(users)
{
}

void ThumbServer::start()
{
    if ( this->listen(QHostAddress::AnyIPv4, m_port) )
        qDebug() << "Server started at " << QHostAddress::AnyIPv4 << ":" << m_port;
    else
        qDebug() << "Can't start server";
}

void ThumbServer::incomingConnection(qintptr handle)
{
    qDebug() << "Receiving a new thumb";

    SocketThreadThumb *thread = new SocketThreadThumb(handle,this->users);
    // when the thread finishes deallocate its objects
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}

ThumbServer::~ThumbServer(){

}
