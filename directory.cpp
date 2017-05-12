#include "directory.h"

list<shared_ptr<Node>> Directory::getNodes() const
{
    return nodes;
}

void Directory::setNodes(const list<shared_ptr<Node>> &value)
{
    nodes = value;
}

void Directory::addNode(const shared_ptr<Node> node)
{
    long n=node->getSize();
    this->nodes.push_back(node);
    this->setSize(this->getSize()+n);
}

Directory::~Directory()
{

}

Directory::Directory()
{

}
