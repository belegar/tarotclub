#ifndef ISERVICE_H
#define ISERVICE_H

#include <string>

class IService
{

public:
    virtual std::string Name() = 0;
    virtual std::string Print() = 0;
};

#endif // ISERVICE_H
