#ifndef MYUDP_H
#define MYUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QIcon>
#include <QNetworkInterface>
#include <QList>
#include <users.h>
#include <map>

class discovery : public QObject
{
    Q_OBJECT

public:
    //explicit discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users, QObject *parent = 0);
    void sendMessage(QJsonObject);
    void sendThumb(QHostAddress addr);
    void sendThumb();

    ~discovery();

    static shared_ptr<discovery> getInstance(QHostAddress addr, quint16 port, shared_ptr<Users> users, QObject *parent = 0)
    {
        static shared_ptr<discovery> instance( new discovery(addr, port, users, parent = 0));
        return instance;
    }

    // Type of UDP messages
    enum MESSAGE { READY, HELLO, THUMBREQ };

public slots:
    void readyRead();
    void notify(MESSAGE msg = READY);
    void garbage();
    void thumbSenderError(QString error);

private:

    QList<QHostAddress> local_addresses;
    shared_ptr<QUdpSocket> socketIn;
    shared_ptr<QUdpSocket> socketOut;
    QHostAddress addr;
    quint16 port;
    shared_ptr<Users> users;
    shared_ptr<QTimer> readyMessageTimer;
    shared_ptr<QTimer> garbageCollectionTimer;

    discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users, QObject *parent = 0);
    discovery(discovery const&);
    void operator=(discovery const&);


};

#endif // MYUDP_H
