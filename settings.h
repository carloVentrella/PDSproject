#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <QSettings>
#include <QVariant>
#include <QString>
#include "user.h"

using namespace std;

class Settings
{
private:
    string destination;
    bool fromAll;
    bool on;

    string root;
    User currentUser;
    int multicastPort;

    Settings();

    Settings(Settings const&);              // Don't Implement
    void operator=(Settings const&);

    string companyName="Bonsai";
    string appName="PDSproject";

public:
    string getDestination() const;
    void setDestination(const string &value);
    bool getFromAll() const;
    void setFromAll(bool value);
    bool getOn() const;
    void setOn(bool value);

    static Settings& getInstance()
    {
        static Settings instance;
        return instance;
    }


    void SaveSetting(const QString &key, const QVariant &variant);
    void LoadSettings();

    string getCompanyName() const;
    void setCompanyName(const string &value);
    string getAppName() const;
    void setAppName(const string &value);
    string getRoot() const;
    void setRoot(const string &value);
    User getCurrentUser() const;
    void setCurrentUser(const User &value);
    int getMulticastPort() const;
    void setMulticastPort(const int &value);
};

#endif // SETTINGS_H
