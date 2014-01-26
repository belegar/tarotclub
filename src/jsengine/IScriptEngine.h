#ifndef I_SCRIPT_ENGINE_H
#define I_SCRIPT_ENGINE_H

#include <string>
#include <vector>

class IScriptEngine
{
public:
    typedef std::vector<std::string> StringList;

    virtual void Initialize() = 0;

    virtual bool Evaluate(const std::string &fileName) = 0;
    virtual std::string Call(const std::string &function, const StringList &args) const = 0;
};

#endif // I_SCRIPT_ENGINE_H
