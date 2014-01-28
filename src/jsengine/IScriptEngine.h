#ifndef I_SCRIPT_ENGINE_H
#define I_SCRIPT_ENGINE_H

#include <string>
#include <vector>
#include "JSValue.h"

class IScriptEngine
{
public:
    typedef std::vector<std::string> StringList;

    virtual void Initialize() = 0;

    virtual bool Evaluate(const std::string &fileName) = 0;
    virtual JSValue Call(const std::string &function, const StringList &args) = 0;
};

#endif // I_SCRIPT_ENGINE_H
