#include "tcptransferserver.h"
#include "socketthread.h"

Server::Server(QHostAddress host, quint16 port, QObject *parent)
    : QTcpServer(parent),
      m_host(host),
      m_port(port)
{
}

void Server::start()
{
    if ( this->listen(QHostAddress::AnyIPv4, m_port) )
        qDebug() << "Server started at " << QHostAddress::AnyIPv4 << ":" << m_port;
    else
        qDebug() << "Can't start server";
}

void Server::incomingConnection(qintptr handle)
{ 

    qDebug() << "Starting a new transfer";

    SocketThread *thread = new SocketThread(handle);
    // when the thread finishes deallocate its objects
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();

}

Server::~Server(){

}
