#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QHostAddress>
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

    // TCP server config
    QHostAddress TCPServerAddr;
    qint16 TCPServerPort;

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
    QHostAddress getTCPServerAddr() const;
    void setTCPServerAddr(const string &ip);
    qint16 getTCPServerPort() const;
    void setTCPServerPort(const qint16 &port);
    bool getOn() const;
    void setOn(bool value);

    //when this function is called, it returns the Settings class if it was previously created;
    //if it the first time the getInstance is called, it calls the constructor
    static Settings& getInstance()
    {
        static Settings instance;
        return instance;
    }

    //this function write the QSetting file
    void SaveSetting(const QString &key, const QVariant &variant);

    //this funcion reads the QSetting file
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
