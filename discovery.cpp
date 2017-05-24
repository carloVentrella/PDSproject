#include "discovery.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <QTimer>
#include <user.h>
#include <memory>


discovery::discovery(QHostAddress addr, quint16 port, shared_ptr<Users> users,  QObject *parent) : QObject(parent)
{
    this->local_addresses = QNetworkInterface::allAddresses();

    this->addr = addr;
    this->port = port;
    this->users = users;

    socketIn = new QUdpSocket(this);
    socketOut = new QUdpSocket(this);

    qDebug("Sockets ok");

    if (socketIn->bind(QHostAddress::AnyIPv4, this->port, QUdpSocket::ShareAddress))
        qDebug("Bind ok");

    if (socketIn->joinMulticastGroup(this->addr))
        qDebug("Joined multicast group");

    // The arrival of a new datagram will trigger the readyRead function
    connect(socketIn,SIGNAL(readyRead()), this, SLOT(readyRead()));

    // Creating a timer that will trigger the notify member function to notify our presence to neighbours
    readyMessageTimer = new QTimer();
    connect(readyMessageTimer, SIGNAL(timeout()), this, SLOT(notify()));

    // creating a timer that will trigger a garbage collection to remove no longer ready neighbours
    garbageCollectionTimer = new QTimer();
    connect(garbageCollectionTimer, SIGNAL(timeout()), this, SLOT(garbage()));

    // Send a ready message without waiting the timer
    notify();

    readyMessageTimer->start(10000);
    garbageCollectionTimer->start(15000);


}


void discovery::readyRead()
{
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;
    string usr;
    string msg;
    string ip;


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

        usr = jsonResponse["USR"].toString().toUtf8().constData();
        msg = jsonResponse["MSG"].toString().toUtf8().constData();
        ip =  sender.toString().toUtf8().constData();

        qDebug("Message from: [%s,%d]", ip.c_str(),senderPort );

        if ( msg == "Ready" ){

            if (this->users->contains(ip)){
                this->users->getUser(ip)->stillAlive();
                return;
            }

           // Add user to the list
           User u;
           u.setIP(ip);
           u.setUsername(usr);

           this->users->addUser(std::make_shared<User>(u));
           qDebug("New user added: [%s]", usr.c_str());

        } else if (msg == "Quitting") {

            // Remove user from the list
            this->users->removeUser(ip);
            qDebug("User [%s] removed", usr.c_str());

        } else {
            qDebug("Unknown command");
        }
    }

}


void discovery::notify(){

    QJsonObject jsonRequest;

    // Example
    // These info should be read from config
    QString user("Mario Rossi");
    QString msg("Ready");

    jsonRequest["USR"]=user;
    jsonRequest["MSG"]=msg;

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
    QByteArray data( doc.toBinaryData() );

    this->socketOut->writeDatagram(data, this->addr, this->port);
    qDebug("Message sent");
}

