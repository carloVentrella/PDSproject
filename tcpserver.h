#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHostAddress>

class Server : public QTcpServer
{
    Q_OBJECT
    public:
    explicit Server(QHostAddress host = QHostAddress::Any,
                        quint16 port      = 0,
                        QObject *parent   = 0);

        ~Server();

    public slots:
        void start();

    protected:
        // virtual function
        void incomingConnection(qintptr handle);

    private:
        QHostAddress m_host;
        quint16      m_port;

};

#endif // SERVER_H
