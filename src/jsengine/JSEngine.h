
#ifndef JSENGINE_H
#define JSENGINE_H

#include "duktape.h"
#include <string>
#include <vector>
#include <iostream>

class IScriptEngine
{
public:

    virtual void Initialize() = 0;

    virtual bool Evaluate(const std::string &fileName) = 0;
    virtual std::string Call(const std::string &function, const std::vector<std::string> &args) const = 0;
};


class JSEngine : public IScriptEngine
{
public:
    typedef std::vector<std::string> StringList;

    JSEngine()
        : mCtx(NULL)
        , mValid(false)
    {

    }

    ~JSEngine()
    {
        Close();
    }
    
    void Initialize()
    {
         mCtx = duk_create_heap_default();
    }
    
    bool Evaluate(const std::string &fileName)
    {
        // Push argument into the stack: file name to evaluate
        duk_push_lstring(mCtx, fileName.c_str(), fileName.size());

        int rc = duk_safe_call(mCtx, JSEngine::WrappedScriptEval, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
        if (rc != DUK_EXEC_SUCCESS)
        {
            PrintError();
            return false;
        }
        else
        {
            return true;
        }
    }

    std::string Call(const std::string &function, const StringList &args) const
    {
        std::string ret;

        return ret;
    }

    void Close()
    {
        if (mCtx)
        {
            duk_destroy_heap(mCtx);
        }
        mValid = false;
    }

private:
    duk_context *mCtx;
    bool mValid;

    static int WrappedScriptEval(duk_context *ctx)
    {
        std::string fileName = duk_get_string(ctx, -1);
        duk_pop(ctx);

        duk_eval_file(ctx, fileName.c_str());
        return 0; // no return values
    }

    /**
     * @brief PrintError
     *
     * Print and pop error.
     *
     */
    void PrintError()
    {

#ifdef TAROT_DEBUG
        if (duk_is_object(mCtx, -1) && duk_has_prop_string(mCtx, -1, "stack"))
        {
            /* FIXME: print error objects specially */
            /* FIXME: pcall the string coercion */
            duk_get_prop_string(mCtx, -1, "stack");
            if (duk_is_string(mCtx, -1))
            {
                std::cout << duk_get_string(mCtx, -1) << std::endl;
                duk_pop_2(mCtx);
                return;
            }
            else
            {
                duk_pop(mCtx);
            }
        }
        duk_to_string(mCtx, -1);
        std::cout << duk_get_string(mCtx, -1) << std::endl;
        duk_pop(mCtx);
#endif
    }

};

#endif // JSENGINE_H
