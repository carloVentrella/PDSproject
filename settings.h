#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>

using namespace std;

class Settings
{
private:
    string destination;
    bool riceviDaTutti;
    bool on;

public:
    Settings();
    string getDestination() const;
    void setDestination(const string &value);
    bool getRiceviDaTutti() const;
    void setRiceviDaTutti(bool value);
    void setDefault(std::string defDest);
    bool getOn() const;
    void setOn(bool value);
};

#endif // SETTINGS_H
