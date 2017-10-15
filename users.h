#ifndef NEIGHBORS_H
#define NEIGHBORS_H

#include<mutex>
#include<map>
#include <memory>
#include <user.h>
#include <QObject>

using namespace std;

class Users: public QObject
{
    Q_OBJECT

public:
    Users();
    void addUser(shared_ptr<User>);
    void removeUser(const QString &key);
    shared_ptr<User> getUser(const QString &key);
    bool contains(const QString &key);
    size_t size();
    void garbageCollection();

    map<QString, shared_ptr<User>> users;

signals:
    void modifiedUsersMap(QString whatsNeeded,bool state);

private:
    mutex mtx;

};

#endif // NEIGHBORS_H
