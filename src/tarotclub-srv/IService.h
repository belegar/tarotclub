#ifndef I_SERVICE_H
#define I_SERVICE_H

#include <string>

class IService
{

public:
    ~IService() { /* Nothing to do */ }

    virtual std::string GetName() = 0;
    virtual void Initialize() = 0;
    virtual void Stop() = 0;

};

#endif // I_SERVICE_H
