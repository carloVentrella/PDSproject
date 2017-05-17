#include "settings.h"

string Settings::getDestination() const
{
    return destination;
}

void Settings::setDestination(const string &value)
{
    destination = value;
}

bool Settings::getRiceviDaTutti() const
{
    return riceviDaTutti;
}

void Settings::setRiceviDaTutti(bool value)
{
    riceviDaTutti = value;
}

void Settings::setDefault(string defDest)
{
    this->setRiceviDaTutti(true);
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
