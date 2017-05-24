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

}

void Users::removeUser(const string &ip){

    mtx.lock();
    users.erase(ip);
    mtx.unlock();

    std::cout << "User ["<< ip <<"] removed" << std::endl;
}

shared_ptr<User> Users::getUser(const string &ip){

    return users.at(ip);
}

size_t Users::size(){
    return users.size();
}

void Users::garbageCollection(){

    std::cout << "Garbage collection started" << std::endl;

    for(  auto const &entry : users ){

        shared_ptr<User> u = (entry.second);

        if (!u->isStillAlive())
            removeUser(u->getIP());

    }

}

bool Users::contains(const string &ip){
    return !( users.find(ip) == users.end());
}
