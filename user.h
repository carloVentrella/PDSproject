#ifndef USER_H
#define USER_H
#include <QObject>
#include <string>
#include <QIcon>
#include <chrono>


using namespace std;
using namespace chrono;

class User: public QObject
{
    Q_OBJECT
private:
    QString username;
    QString IP;
    QIcon thumbnail;
    seconds silentFrom;
    bool thumbnailChanged;

public:
    User(){}
    QString getIP() const;
    void setIP(const QString &value);
    QString getUsername() const;
    void setUsername(const QString &value);
    QIcon getThumbnail() const;
    bool isThumbnailChanged() const;
    void setThumbnailChanged(bool value);
    void setThumbnail(const QIcon &value);
    seconds getSilentFrom() const;
    bool isStillAlive() const;
    void stillAlive();

signals:
    void modifiedThumb(const QIcon &value, QString username);

};

#endif // USER_H
