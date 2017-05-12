#include "user.h"

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

User::User()
{

}
