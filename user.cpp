#include "user.h"
#include <iostream>

#include <qdebug.h>

QString User::getIP() const
{
    return IP;
}

void User::setIP(const QString &value)
{
    IP = value;
}

QString User::getUsername() const
{
    return username;
}

void User::setUsername(const QString &value)
{
    username = value;
}

QIcon User::getThumbnail() const
{
    return thumbnail;
}

bool User::isThumbnailChanged() const
{
    return thumbnailChanged;
}

void User::setThumbnailChanged(bool value)
{
    this->thumbnailChanged = value;
    emit modifiedThumb(this->getThumbnail(),this->getUsername());
}

void User::setThumbnail(const QIcon &value)
{
    thumbnail = value;
    setThumbnailChanged(true);
}

void User::stillAlive()
{
    this->silentFrom = duration_cast< seconds >( system_clock::now().time_since_epoch());

}

bool User::isStillAlive() const
{
    chrono::seconds now = duration_cast< seconds >( system_clock::now().time_since_epoch());
    return now < (this->silentFrom + chrono::seconds(10));

}


seconds User::getSilentFrom() const
{
    return this->silentFrom;
}

