#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "settings.h"
#include "askconfirmationwindow.h"
#include "loadingwheel.h"

class SocketThread : public QThread
{
    Q_OBJECT
    public:
        SocketThread(qintptr descriptor, QObject *parent = 0);
        ~SocketThread();

    protected:
        // virtual function
        void run();

    signals:
        void onFinishRecieved();

        //this signal has to be used when i need to change the value in the loading wheel
        void valueChanged(float value);

    private slots:
        // Function called when there are bytes to be read
        void onReadyRead();
        // Function called if the socket disconnect
        void onDisconnected();
        // Function called if there is an error
        void onError(QAbstractSocket::SocketError);

public slots:
        void reception();

    private:
        qintptr m_socketDescriptor;
        QTcpSocket *m_socket;
        qint32      m_blockSize;
        quint32     m_bytesRead;
        bool        m_readingFile;

        QString fileName;
        QString type;
};

#endif // SOCKETTHREAD_H
