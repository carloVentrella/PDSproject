#include "user.h"
#include <iostream>

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

void User::setThumbnail(const QIcon &value)
{
    thumbnail = value;
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
