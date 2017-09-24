#ifndef MYUDP_H
#define MYUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QIcon>
#include <QNetworkInterface>
#include <QList>
#include <users.h>

class discovery : public QObject
{
    Q_OBJECT

public:
    explicit discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users, QObject *parent = 0);
    void sendMessage(QJsonObject);

    discovery(const discovery& source);

signals:
    void modifiedThumb(const QIcon &value, string username);

public slots:
    void readyRead();
    void notify(bool thumb=false);
    void garbage();

private:
    QList<QHostAddress> local_addresses;
    QUdpSocket *socketIn;
    QUdpSocket *socketOut;
    QHostAddress addr;
    quint16 port;
    shared_ptr<Users> users;
    QTimer *readyMessageTimer;
    QTimer *garbageCollectionTimer;

};

#endif // MYUDP_H
