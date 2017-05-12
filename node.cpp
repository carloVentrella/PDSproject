#include "node.h"

long Node::getSize() const
{
    return size;
}

void Node::setSize(long value)
{
    size = value;
}

string Node::getName() const
{
    return name;
}

void Node::setName(const string &value)
{
    name = value;
}

string Node::getLocation() const
{
    return location;
}

void Node::setLocation(const string &value)
{
    location = value;
}

Node::Node()
{


}

Node::~Node()
{

}
