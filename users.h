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
    void removeUser(const string &key);
    shared_ptr<User> getUser(const string &key);
    bool contains(const string &key);
    size_t size();
    void garbageCollection();

    map<string, shared_ptr<User>> users;

signals:
    void modifiedUsersMap(string whatsNeeded,int whichUser, bool state);

private:
    mutex mtx;

};

#endif // NEIGHBORS_H
