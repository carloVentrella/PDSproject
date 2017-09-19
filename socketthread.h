#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>

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
        bool        m_readingFile;

        QString fileName;
        QString type;

};

#endif // SOCKETTHREAD_H
