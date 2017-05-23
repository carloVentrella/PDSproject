#include "settings.h"

string Settings::getDestination() const
{
    return destination;
}

void Settings::setDestination(const string &value)
{
    destination = value;
}

bool Settings::getFromAll() const
{
    return fromAll;
}

void Settings::setFromAll(bool value)
{
    fromAll = value;
}

void Settings::setDefault(string defDest)
{
    this->setFromAll(true);
    this->setDestination(defDest);
    this->setOn(true);
}

bool Settings::getOn() const
{
    return on;
}

void Settings::setOn(bool value)
{
    on = value;
}

Settings::Settings()
{

}
