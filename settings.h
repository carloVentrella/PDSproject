#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>

using namespace std;

class Settings
{
private:
    string destination;
    bool fromAll;
    bool on;

public:
    Settings();
    string getDestination() const;
    void setDestination(const string &value);
    bool getFromAll() const;
    void setFromAll(bool value);
    void setDefault(std::string defDest);
    bool getOn() const;
    void setOn(bool value);
};

#endif // SETTINGS_H
