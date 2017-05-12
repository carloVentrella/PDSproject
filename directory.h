#ifndef DIRECTORY_H
#define DIRECTORY_H
#include "node.h"
#include <list>
#include <memory>

using namespace std;

class Directory:  public Node
{
private:
    list<shared_ptr<Node>> nodes;
public:
    Directory();
    list<shared_ptr<Node>> getNodes() const;
    void setNodes(const list<shared_ptr<Node>> &value);

    void addNode(const shared_ptr<Node> node);
    ~Directory();
};

#endif // DIRECTORY_H
