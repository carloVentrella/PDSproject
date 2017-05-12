#ifndef USER_H
#define USER_H
#include <string>
#include <QIcon>

using namespace std;

class User
{
private:
    std::string username;
    std::string IP;
    QIcon thumbnail;


public:
    User();



    std::string getIP() const;
    void setIP(const std::string &value);
    std::string getUsername() const;
    void setUsername(const std::string &value);
    QIcon getThumbnail() const;
    void setThumbnail(const QIcon &value);
};

#endif // USER_H
