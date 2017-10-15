#ifndef THUMBSERVER_H
#define THUMBSERVER_H

#include <QTcpServer>
#include <QHostAddress>
#include <memory>
#include "users.h"

class ThumbServer : public QTcpServer
{
    Q_OBJECT
    public:
    explicit ThumbServer(QHostAddress host = QHostAddress::Any,
                        quint16 port      = 0,
                        shared_ptr<Users> users = 0,
                        QObject *parent   = 0);

        ~ThumbServer();

    public slots:
        void start();

    protected:
        // virtual function
        void incomingConnection(qintptr handle);

    private:
        QHostAddress m_host;
        quint16      m_port;
        shared_ptr<Users> users;

};

#endif // THUMBSERVER_H
