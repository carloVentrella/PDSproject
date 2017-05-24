#ifndef NEIGHBORS_H
#define NEIGHBORS_H

#include<mutex>
#include<map>
#include <memory>
#include <user.h>

using namespace std;

class Users
{
public:
    Users();
    void addUser(shared_ptr<User>);
    void removeUser(const string &key);
    shared_ptr<User> getUser(const string &key);
    bool contains(const string &key);
    size_t size();
    void garbageCollection();

private:
    mutex mtx;
    map<string, shared_ptr<User>> users;

};

#endif // NEIGHBORS_H
