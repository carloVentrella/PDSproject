#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QHostAddress>
#include "user.h"
#include <QIcon>
#include <memory>

using namespace std;

class Settings
{
private:
    string destination;
    bool fromAll;
    bool on;
    string root;
    shared_ptr<User> currentUser;
    int multicastPort;

    //this variable is needed to find the path of the user's thumbnail
    QString thumbPath;

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
    shared_ptr<User> getCurrentUser() const;
    void setCurrentUser(const shared_ptr<User> value);
    int getMulticastPort() const;
    void setMulticastPort(const int &value);

    //methods to set and get the user thumbnail
    QString getThumbPath() const;
    void setThumbPath(const QString &value);
};

#endif // SETTINGS_H
