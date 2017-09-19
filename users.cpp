#include "users.h"
#include <iostream>

Users::Users(){

}

void Users::addUser(shared_ptr<User> u){
    mtx.lock();
    users.insert(make_pair(u->getIP(),u));
    mtx.unlock();

    u->stillAlive();

    std::cout << "User ["<< u->getIP() <<"] added" << std::endl;
    emit modifiedUsersMap(u->getIP(),(users.size())-1,true);
}

void Users::removeUser(const string &ip){

    mtx.lock();
    int pos=0;
    string username;
    for(map<string,shared_ptr<User>>::iterator iter=users.begin();iter!=users.end();iter++)
    {
        if(iter->first==ip){
            username=iter->second->getUsername();
            break;
        }
        pos++;
    }
    users.erase(ip);
    mtx.unlock();

    std::cout << "User ["<< ip <<"] removed" << std::endl;

    emit modifiedUsersMap(username, pos,false);
}

shared_ptr<User> Users::getUser(const string &ip){

    return users.at(ip);
}

size_t Users::size(){
    return users.size();
}

void Users::garbageCollection(){

    std::cout << "Garbage collection started" << std::endl;

    mtx.lock();
    int pos=0;
    string username;
    string ip;

    std::cout << "Size: " << users.size() << std::endl;

    for(map<string,shared_ptr<User>>::iterator iter=users.begin();iter!=users.end();)
    {
        if(!iter->second->isStillAlive()){
            username=iter->second->getUsername();
            ip=iter->second->getIP();
            iter = users.erase(iter);
            emit modifiedUsersMap(username, pos,false);
        }else{
            ++iter;
        }

        pos++;

    }

    mtx.unlock();

    std::cout << "User ["<< username <<"] removed" << std::endl;

    std::cout << "Garbage collection finished" << std::endl;

}

bool Users::contains(const string &ip){
    return !( users.find(ip) == users.end());
}
