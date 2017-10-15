#include "users.h"
#include <iostream>
#include "qdebug.h"

Users::Users(){

}

void Users::addUser(shared_ptr<User> u){
    mtx.lock();
    users.insert(make_pair(u->getIP(),u));
    mtx.unlock();

    u->stillAlive();

    qDebug() << "User ["<< u->getIP() <<"] added";
    emit modifiedUsersMap(u->getIP(),true);
}

void Users::removeUser(const QString &ip){

    mtx.lock();
    QString username;
    for(map<QString,shared_ptr<User>>::iterator iter=users.begin();iter!=users.end();iter++)
    {
        if(iter->first==ip){
            username=iter->second->getUsername();
            break;
        }
    }
    qDebug() << "Removing user with ip ["<< ip <<"]";
    users.erase(ip);
    mtx.unlock();

    emit modifiedUsersMap(username, false);
}

shared_ptr<User> Users::getUser(const QString &ip){

    if (users.find(ip) == users.end()){
        qDebug() << "This IP is not in the user list";
        return nullptr;
    }

    return users.at(ip);
}

size_t Users::size(){
    return users.size();
}

void Users::garbageCollection(){

    std::cout << "Garbage collection started" << std::endl;

    mtx.lock();
    QString username;
    QString ip;

    std::cout << "Size: " << users.size() << std::endl;

    for(map<QString,shared_ptr<User>>::iterator iter=users.begin();iter!=users.end();)
    {
        if(!iter->second->isStillAlive()){
            username=iter->second->getUsername();
            ip=iter->second->getIP();
            iter = users.erase(iter);
            emit modifiedUsersMap(username,false);
        }else{
            ++iter;
        }

    }

    mtx.unlock();

    qDebug() << "User ["<< username <<"] removed";

    qDebug() << "Garbage collection finished";

}

bool Users::contains(const QString &ip){
    return !( users.find(ip) == users.end());
}
