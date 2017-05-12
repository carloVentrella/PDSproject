#ifndef FILE_H
#define FILE_H
#include "node.h"


class File: public Node
{
private:
    bool symbolic;
public:
    File();
    bool getSymbolic() const;
    void setSymbolic(bool value);
    ~File();
};

#endif // FILE_H
