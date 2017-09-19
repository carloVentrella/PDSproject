#include "tcpserver.h"
#include "socketthread.h"

Server::Server(QHostAddress host, quint16 port, QObject *parent)
    : QTcpServer(parent),
      m_host(host),
      m_port(port)
{
    // your settings init there
}

void Server::start()
{
    if ( this->listen(m_host, m_port) )
        qDebug() << "Server started at " << m_host.toString() << ":" << m_port;
    else
        qDebug() << "Can't start server";
}

void Server::incomingConnection(qintptr handle)
{
    qDebug() << "incomingConnection = " << handle;
    SocketThread *thread = new SocketThread(handle);

    // when the thread finishes deallocate its objects
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

Server::~Server(){

}
