#ifndef SOCKETTHREADTHUMB_H
#define SOCKETTHREADTHUMB_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <memory>
#include "users.h"

class SocketThreadThumb : public QThread
{
    Q_OBJECT
    public:
        SocketThreadThumb(qintptr descriptor, shared_ptr<Users> users, QObject *parent = 0);
        ~SocketThreadThumb();
    protected:
        // virtual function
        void run();

    private slots:
        // Function called when there are bytes to be read
        void onReadyRead();
        // Function called if the socket disconnect
        void onDisconnected();
        // Function called if there is an error
        void onError(QAbstractSocket::SocketError);

    private:
        qintptr m_socketDescriptor;
        QTcpSocket *m_socket;
        qint32      m_blockSize;
        quint32     m_bytesRead;
        shared_ptr<Users> users;
        shared_ptr<User> usr;

    };

#endif // SOCKETTHREADTHUMB_H
