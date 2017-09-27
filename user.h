#ifndef USER_H
#define USER_H
#include <string>
#include <QIcon>
#include <chrono>


using namespace std;
using namespace chrono;

class User
{
private:
    string username;
    string IP;
    QIcon thumbnail;
    seconds silentFrom;
    bool thumbnailChanged;

public:
    User();
    string getIP() const;
    void setIP(const string &value);
    string getUsername() const;
    void setUsername(const string &value);
    QIcon getThumbnail() const;
    bool isThumbnailChanged() const;
    void setThumbnailChanged(bool value);
    void setThumbnail(const QIcon &value);
    seconds getSilentFrom() const;
    bool isStillAlive() const;
    static seconds MAX_SILENT;
    void stillAlive();
};

#endif // USER_H
