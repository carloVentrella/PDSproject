#include "user.h"
#include <iostream>

#include <qdebug.h>

std::string User::getIP() const
{
    return IP;
}

void User::setIP(const std::string &value)
{
    IP = value;
}

std::string User::getUsername() const
{
    return username;
}

void User::setUsername(const std::string &value)
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
}

void User::setThumbnail(const QIcon &value)
{
    thumbnail = value;
    this->thumbnailChanged = true;
}

void User::stillAlive()
{
    this->silentFrom = duration_cast< seconds >( system_clock::now().time_since_epoch());

}

bool User::isStillAlive() const
{
    chrono::seconds now = duration_cast< seconds >( system_clock::now().time_since_epoch());
    return now < (this->silentFrom + User::MAX_SILENT);

}


seconds User::getSilentFrom() const
{
    return this->silentFrom;
}

User::User()
{

}
