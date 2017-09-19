#ifndef CLIENT_H
#define CLIENT_H

#include <QHostAddress>
#include <QFile>
#include <QList>
#include <memory>
#include <QFileInfoList>

class Client : QObject
{
    Q_OBJECT
    public:
        explicit Client(QHostAddress host = QHostAddress::Any,
                        quint16 port      = 0,
                        QObject *parent   = 0) :  m_host(host), m_port(port) {}
        void send();

    private:
        QHostAddress m_host;
        qint16 m_port;
        QList< std::shared_ptr<QFile> > fileList;
        void createFileList( QFileInfoList filenames );

};

#endif // CLIENT_H
