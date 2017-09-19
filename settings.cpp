#include "settings.h"

#include <iostream>

string Settings::getDestination() const
{
    return destination;
}

void Settings::setDestination(const string &value)
{
    destination = value;
    this->SaveSetting("downloadDir", QString::fromStdString(value));
}

bool Settings::getFromAll() const
{
    return fromAll;
}

void Settings::setFromAll(bool value)
{

    fromAll = value;
    this->SaveSetting("downloadWithoutConfirmation",value);

}

bool Settings::getOn() const
{
    return on;
}

void Settings::setOn(bool value)
{
    on = value;
    this->SaveSetting("public", value);
}

void Settings::SaveSetting(const QString &key, const QVariant &variant)
{
    QSettings settings(QString::fromStdString(companyName), QString::fromStdString(appName));
    settings.setValue(key, variant);
}

void Settings::LoadSettings()
{
    //finding the root of the fileSystemTree
    //it is necessary to discover the username
    std::string user=qgetenv("USER").toStdString();
    std::string path="/home/";
    std::string thumb=":/thumbnails/3.png";
    path.append(user);


    QSettings settings(QString::fromStdString(companyName), QString::fromStdString(appName));
    destination=settings.value("downloadDir", QString::fromStdString(path)).toString().toStdString();

    root=settings.value("rootDir", QString::fromStdString(path)).toString().toStdString();

    fromAll=settings.value("downloadWithoutConfirmation", true).toBool();

    on=settings.value("public", true).toBool();

    currentUser.setIP(settings.value("user/IP", "239.255.43.21").toString().toStdString());
    currentUser.setUsername(settings.value("user/username",QString::fromStdString(user)).toString().toStdString());
    currentUser.setThumbnail(QIcon(thumb.c_str()));
    multicastPort=settings.value("discovery/multicastPort", 45454).toInt();
    //THUMBNAIL HERE !!!
    //MAXSILENT AND GARBAGE COLLECTOR TIMER
}

string Settings::getRoot() const
{
    return root;
}

void Settings::setRoot(const string &value)
{
    root = value;
    this->SaveSetting("rootDir", QString::fromStdString(value));
}

User Settings::getCurrentUser() const
{
    return currentUser;
}

void Settings::setCurrentUser(const User &value)
{
    currentUser = value;
    this->SaveSetting("user/IP", QString::fromStdString(
                          value.getIP()));
    this->SaveSetting("user/username", QString::fromStdString(value.getUsername()));
}

int Settings::getMulticastPort() const
{
    return multicastPort;
}

void Settings::setMulticastPort(const int &value)
{
    multicastPort = value;
    this->SaveSetting("discovery/multicastPort", value);
}

Settings::Settings()
{
    LoadSettings();
}

string Settings::getAppName() const
{
    return appName;
}

void Settings::setAppName(const string &value)
{
    appName = value;
}

string Settings::getCompanyName() const
{
    return companyName;
}

void Settings::setCompanyName(const string &value)
{
    companyName = value;
}

QHostAddress Settings::getTCPServerAddr() const
{
    return TCPServerAddr;
}

void Settings::setTCPServerAddr(const string &ip)
{
    TCPServerAddr = QHostAddress( QString::fromStdString(ip) );
}

qint16 Settings::getTCPServerPort() const
{
    return TCPServerPort;
}

void Settings::setTCPServerPort(const qint16 &port)
{
    TCPServerPort = port;
}
