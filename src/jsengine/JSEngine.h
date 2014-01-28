
#ifndef JSENGINE_H
#define JSENGINE_H

#include <string>
#include <vector>
#include <iostream>
#include "duktape.h"
#include "IScriptEngine.h"


class JSEngine : public IScriptEngine
{
public:
    JSEngine();
    ~JSEngine();
    
    void Initialize();
    bool Evaluate(const std::string &fileName);
    JSValue Call(const std::string &function, const IScriptEngine::StringList &args);
    void Close();

private:
    duk_context *mCtx;
    bool mValid;

    static int WrappedScriptEval(duk_context *ctx);
    static int WrappedScriptCall(duk_context *ctx);

    void PrintError() const;
    void PrintTop() const;

};

#endif // JSENGINE_H
