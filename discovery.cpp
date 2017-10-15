#include "discovery.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QBuffer>
#include <thread>
#include <chrono>
#include <iostream>
#include <QTimer>
#include <memory>
#include <QDateTime>
#include <QThread>

#include "user.h"
#include "settings.h"
#include "tcpthumbserver.h"
#include "thumbsender.h"

discovery::discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users,  QObject *parent) : QObject(parent)
{

    this->local_addresses = QNetworkInterface::allAddresses();

    this->addr = addr;
    this->port = port;
    this->users = users;

    // UDP sockets

    socketIn = std::make_shared<QUdpSocket>(new QUdpSocket(this));
    socketOut = std::make_shared<QUdpSocket>(new QUdpSocket(this));

    qDebug() << "Sockets ok";

    if (socketIn->bind(QHostAddress::AnyIPv4, this->port, QUdpSocket::ShareAddress))
        qDebug() << "Bind ok";

    if (socketIn->joinMulticastGroup(this->addr))
        qDebug() << "Joined multicast group";

    socketIn->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,2000000);

    // The arrival of a new datagram will trigger the readyRead function
    connect(socketIn.get(),SIGNAL(readyRead()), this, SLOT(readyRead()));

    // Creating a timer that will trigger the notify member function to notify our presence to neighbours
    readyMessageTimer = std::make_shared<QTimer>(new QTimer());
    connect(readyMessageTimer.get(), SIGNAL(timeout()), this, SLOT(notify()));

    // creating a timer that will trigger a garbage collection to remove no longer ready neighbours
    garbageCollectionTimer = std::make_shared<QTimer>(new QTimer());
    connect(garbageCollectionTimer.get(), SIGNAL(timeout()), this, SLOT(garbage()));

    // Send a ready message without waiting the timer
    notify();

    readyMessageTimer->start(3000);
    garbageCollectionTimer->start(7000);

}



void discovery::readyRead()
{
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;
    QString username;
    QString msg;
    QString ip;
    QIcon thumb;


    while (socketIn->hasPendingDatagrams()) {

        // Return the size of the first pending datagram
        buffer.resize(socketIn->pendingDatagramSize());

        socketIn->readDatagram(buffer.data(), buffer.size(),
                             &sender, &senderPort);

        // Ignore own datagram
        if ((local_addresses.contains(sender)) && (senderPort == this->socketOut->localPort())) {
            return;
        }

        // Create json from received data
        QJsonDocument doc = QJsonDocument::fromBinaryData(buffer);
        QJsonObject jsonResponse = doc.object();

        username = jsonResponse["USR"].toString().toUtf8().constData();
        msg = jsonResponse["MSG"].toString().toUtf8().constData();

        ip =  sender.toString();

        qDebug() << "Message from: ["<< ip << "," << senderPort << "]";

        if ( msg == "Ready" ){

            if (this->users->contains(ip)){

                shared_ptr<User> user(this->users->getUser(ip));

                // update user lifetime
                user->stillAlive();
                return;
            }

           // Add user to the list
           shared_ptr<User> user(new User());
           user->setIP(ip);
           user->setUsername(username);

           this->users->addUser(user);
           qDebug() << "New user added: [" << username << "]";

           // if a new user has been added I have to send a new notification...
           this->notify();
           // ...and my thumbnail
           sendThumb(QHostAddress( user->getIP() ));

        } else if (msg == "Quitting") {

            // Remove user from the list
            this->users->removeUser(ip);
            qDebug() << "User " << username << "removed" << username;

        } else {

            qDebug() << "Unknown command";
        }
    }

}

void discovery::notify(){

    QJsonObject jsonRequest;

    // Example
    // These info should be read from config

    shared_ptr<User> curUsr(Settings::getInstance().getCurrentUser());

    // don't notify if private mode is active
    if ( !Settings::getInstance().getOn() )
        return;

    QString user = curUsr->getUsername();
    QString msg("Ready");

    jsonRequest["USR"]=user;
    jsonRequest["MSG"]=msg;

    sendMessage(jsonRequest);

    bool isThumbnailChanged = curUsr->isThumbnailChanged();
    qDebug() << "isThumbnailChanged?" << isThumbnailChanged;

    if (isThumbnailChanged){
        // send the new thumb to all clients
        curUsr->setThumbnailChanged(false);
        sendThumb();
    }

}

void discovery::garbage(){

    this->users->garbageCollection();
}

void discovery::sendMessage(QJsonObject jsonRequest)
{
    if (jsonRequest.empty()){
        qDebug() << "jsonRequest is empty.";
        return;
    }

    // create the json binary representation
    QJsonDocument doc(jsonRequest);
    QByteArray data(doc.toBinaryData());

    if(this->socketOut->writeDatagram(data, data.size(), this->addr, this->port)==-1){
        // We may reach this point if the datagram is too big
        // The maximum size of a datagram is highly platform indipendent
        qDebug() << "Error while sending datagram";
        qDebug() << "Error:" << this->socketOut->error();

    }else{
        qDebug() << "Datagram sent ("<< data.size() <<")";
    }

}

void discovery::sendThumb(){

    // send thumb to all clients

    for (auto const &u : this->users->users){
        shared_ptr<User> usr(u.second);
        sendThumb(QHostAddress(usr->getIP()));
    }

}

void discovery::sendThumb(QHostAddress address){

    // start a new thread that send the thumbnail to a specific host

    QThread* thread = new QThread;
    qint16 port = 5556;
    ThumbSender *thumbSender = new ThumbSender(address, port, users);
    thumbSender->moveToThread(thread);
    connect(thumbSender, SIGNAL (error(QString)), this, SLOT (thumbSenderError(QString)));
    connect(thread, SIGNAL (started()), thumbSender, SLOT (process()));
    connect(thumbSender, SIGNAL (finished()), thread, SLOT (quit()));
    connect(thumbSender, SIGNAL (finished()), thumbSender, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();

}

void discovery::thumbSenderError(QString error){
    qDebug() << "Error in thumbSender";
    qDebug() << error;
}

discovery::~discovery()
{
    socketIn->leaveMulticastGroup(this->addr);
    socketIn->close();
    socketOut->close();
}
