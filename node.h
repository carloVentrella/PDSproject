#ifndef NODE_H
#define NODE_H
#include <string>

using namespace std;

class Node
{
private:
    string name;
    string location;
    long size;

public:
    Node();
    virtual ~Node();
    long getSize() const;
    void setSize(long value);
    string getName() const;
    void setName(const string &value);
    string getLocation() const;
    void setLocation(const string &value);
};

#endif // NODE_H
