#include "settings.h"

#include <iostream>

QString Settings::getDestination() const
{
    return destination;
}

void Settings::setDestination(const QString &value)
{
    destination = value;
    this->SaveSetting("downloadDir", value);
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
    QSettings settings(companyName, appName);
    settings.setValue(key, variant);
}

void Settings::LoadSettings()
{
    //finding the root of the fileSystemTree
    //it is necessary to discover the username
    QString user=qgetenv("USER");
    QString path("/home/");
    QString thumb(":/thumbnails/3.png");
    path.append(user);


    QSettings settings(companyName, appName);

    destination = settings.value("downloadDir", path).toString();
    root=settings.value("rootDir", path).toString();
    fromAll=settings.value("downloadWithoutConfirmation", true).toBool();
    on=settings.value("public", true).toBool();
    thumbPath=settings.value("user/icon", thumb).toString();

    currentUser = std::make_shared<User>();

    currentUser->setIP(settings.value("user/IP", "239.255.43.21").toString());
    currentUser->setUsername(settings.value("user/username",user).toString());
    currentUser->setThumbnail(QIcon(thumbPath));
    multicastPort=settings.value("discovery/multicastPort", 45454).toInt();

    TCPServerPort = settings.value("TCPServerPort",5555).toInt();
}

QString Settings::getRoot() const
{
    return root;
}

void Settings::setRoot(const QString &value)
{
    root = value;
    this->SaveSetting("rootDir",value);
}

shared_ptr<User> Settings::getCurrentUser() const
{
    return currentUser;
}

void Settings::setCurrentUser(const shared_ptr<User> value)
{
    currentUser = value;
    this->SaveSetting("user/IP", value->getIP());
    this->SaveSetting("user/username", value->getUsername());
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

QString Settings::getThumbPath() const
{
    return thumbPath;
}

void Settings::setThumbPath(const QString &value)
{
    thumbPath = value;
    this->SaveSetting("user/icon", value);
    this->currentUser->setThumbnail(QIcon(value));
}

Settings::Settings()
{
    LoadSettings();
}

QString Settings::getAppName() const
{
    return appName;
}

void Settings::setAppName(const QString &value)
{
    appName = value;
}

QString Settings::getCompanyName() const
{
    return companyName;
}

void Settings::setCompanyName(const QString &value)
{
    companyName = value;
}

QHostAddress Settings::getTCPServerAddr() const
{
    return TCPServerAddr;
}

void Settings::setTCPServerAddr(const QString &ip)
{
    TCPServerAddr = QHostAddress( ip );
}

qint16 Settings::getTCPServerPort() const
{
    return TCPServerPort;
}

void Settings::setTCPServerPort(const qint16 &port)
{
    TCPServerPort = port;
}
