#include "discovery.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QBuffer>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <QTimer>
#include <user.h>
#include <memory>
#include <QDateTime>
#include <settings.h>

discovery::discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users,  QObject *parent) : QObject(parent)
{

    this->local_addresses = QNetworkInterface::allAddresses();

    this->addr = addr;
    this->port = port;
    this->users = users;

    socketIn = std::make_shared<QUdpSocket>(new QUdpSocket(this));
    socketOut = std::make_shared<QUdpSocket>(new QUdpSocket(this));

    qDebug("Sockets ok");

    if (socketIn->bind(QHostAddress::AnyIPv4, this->port, QUdpSocket::ShareAddress))
        qDebug("Bind ok");

    if (socketIn->joinMulticastGroup(this->addr))
        qDebug("Joined multicast group");

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

    readyMessageTimer->start(10000);
    garbageCollectionTimer->start(150000);


}


void discovery::readyRead()
{
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;
    string username;
    string msg;
    string ip;
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


        if (jsonResponse.contains("THUMB")){
            // Extract image
            QPixmap p;
            QByteArray encoded = jsonResponse["THUMB"].toString().toLatin1();
            p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
            thumb.addPixmap(p);

            qDebug("Thumb received");

        }

        ip =  sender.toString().toUtf8().constData();

        qDebug("Message from: [%s,%d]", ip.c_str(),senderPort );

        if ( msg == "Ready" ){

            if (this->users->contains(ip)){

                shared_ptr<User> user(this->users->getUser(ip));

                // update thumbnail if present
                if (!thumb.isNull())
                {
                    user->setThumbnail(thumb);

                    //this signal has to be emitted to update the userswindow
                    emit modifiedThumb(thumb, user->getUsername());
                }

                // update user lifetime
                user->stillAlive();
                return;
            }

           // Add user to the list
           shared_ptr<User> user(new User());
           user->setIP(ip);
           user->setUsername(username);
           // update thumbnail if present
           if (!thumb.isNull())
           {
               user->setThumbnail(thumb);
           }
           this->users->addUser(user);
           qDebug("New user added: [%s]", username.c_str());

           // if a new user has been added I have to send a new notification
           // to send him my thumbnail
           this->notify(true);

        } else if (msg == "Quitting") {

            // Remove user from the list
            this->users->removeUser(ip);
            qDebug("User [%s] removed", username.c_str());

        } else {
            qDebug("Unknown command");
        }
    }

}


void discovery::notify(bool thumb){

    QJsonObject jsonRequest;

    // Example
    // These info should be read from config

    User curUsr = Settings::getInstance().getCurrentUser();

    QString user = QString::fromStdString(curUsr.getUsername());
    QIcon icn = curUsr.getThumbnail();
    QString msg("Ready");

    jsonRequest["USR"]=user;
    jsonRequest["MSG"]=msg;

    if (thumb){

        // extracts array of bytes from QIcon
        QByteArray data;
        QBuffer buffer { &data };
        QList<QSize> sizes = icn.availableSizes();
        QPixmap pixmap = icn.pixmap(icn.actualSize(QSize(100,100)));
        buffer.open(QIODevice::WriteOnly);
        if (!pixmap.save(&buffer, "PNG")){
            qDebug("Cannot send thumb");
            return;
        }
        QByteArray encoded = buffer.data().toBase64();
        jsonRequest.insert("THUMB",QJsonValue(QString(encoded)));

    }

    sendMessage(jsonRequest);

}

void discovery::garbage(){

    this->users->garbageCollection();
}

void discovery::sendMessage(QJsonObject jsonRequest)
{
    if (jsonRequest.empty()){
        qDebug("jsonRequest is empty.");
        return;
    }

    // create the json binary representation
    QJsonDocument doc(jsonRequest);
    QByteArray data(doc.toBinaryData());

    if(this->socketOut->writeDatagram(data, this->addr, this->port)==-1){
        // We may reach this point if the datagram is too big
        // The maximum size of a datagram is highly platform indipendent
        qDebug("Error while sending datagram");
        qDebug() << "Error:" << this->socketOut->error();

    }else{
        qDebug() << "Datagram sent ("<< data.size() <<")";
    }

}

