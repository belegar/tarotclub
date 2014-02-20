
#include "JSEngine.h"


/*****************************************************************************/
extern "C" void my_panic_handler(int code, const char *msg)
{
    (void)code;
    (void)msg;
    /* Your panic handling.  Must not return. */
    volatile int i = 0;
    i++;
    while(1);
}
/*****************************************************************************/
JSEngine::JSEngine()
    : mCtx(NULL)
    , mValid(false)
{

}
/*****************************************************************************/
JSEngine::~JSEngine()
{
    Close();
}
/*****************************************************************************/
void JSEngine::Initialize()
{
     mCtx = duk_create_heap_default();
     if (mCtx != NULL)
     {
         mValid = true;
     }
}
/*****************************************************************************/
bool JSEngine::Evaluate(const std::string &fileName)
{
    if (!mValid)
    {
        return false;
    }

    // Push argument into the stack: file name to evaluate
    duk_push_lstring(mCtx, fileName.c_str(), fileName.size());

    int rc = duk_safe_call(mCtx, JSEngine::WrappedScriptEval, 1 /*nargs*/, 0 /*nret*/, DUK_INVALID_INDEX);
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
/*****************************************************************************/
JSValue JSEngine::Call(const std::string &function, const IScriptEngine::StringList &args)
{
    JSValue retval; // default is invalid value

    if (!mValid)
    {
        return retval;
    }

    // Push the Ecmascript global object to the value stack
    duk_push_global_object(mCtx);
    duk_get_prop_string(mCtx, -1 /*index*/, function.c_str()); // actually gets this function name

#ifdef TAROT_DEBUG
    PrintTop();
#endif

    if (!duk_is_undefined(mCtx, -1))
    {
        // Push all the arguments
        for (size_t i = 0; i < args.size(); i++)
        {
            duk_push_string(mCtx, args[i].c_str());
        }

        int rc = duk_safe_call(mCtx, JSEngine::WrappedScriptCall, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
        if (rc != DUK_EXEC_SUCCESS)
        {
            PrintError();
        }
        else
        {
            if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
            {
                std::string value = duk_get_string(mCtx, -1);
                retval = JSValue(value);
            }
            if (duk_check_type(mCtx, -1, DUK_TYPE_BOOLEAN))
            {
                bool value = false;
                if (duk_get_boolean(mCtx, -1))
                {
                    value = true;
                }
                retval = JSValue(value);
            }
        }
    }

    // Context garbage collector: remove all stack elements
    while (duk_get_top(mCtx) > 0)
    {
        duk_pop(mCtx);
    }

    return retval;
}
/*****************************************************************************/
void JSEngine::Close()
{
    if (mCtx)
    {
        duk_destroy_heap(mCtx);
    }
    mValid = false;
}
/*****************************************************************************/
int JSEngine::WrappedScriptEval(duk_context *ctx)
{
    std::string fileName = duk_get_string(ctx, -1);
    duk_pop(ctx);

    duk_eval_file(ctx, fileName.c_str());

    // The result of the evaluation is pushed on top of the value stack. Here we don't need th evaluation result, so we pop the value off the stack.
    duk_pop(ctx);

    return 0; // no return values
}
/*****************************************************************************/
/**
 * @brief WrappedScriptCall
 *
 * context must be initialized:
 * [ global_object "function" arg1 ... argN ]
 *
 * @param ctx
 * @return
 */
int JSEngine::WrappedScriptCall(duk_context *ctx)
{
    int args = duk_get_top(ctx) - 2; // number of args is the stack size minus contex and function

    // Call the function
    duk_call(ctx, args/*nargs*/);

    // After the call, the stack is: [ global_object return value ]
    // The return value is set to undefined type if the function called returns nothing

    return 1; // number of return values
}
/*****************************************************************************/
/**
 * @brief PrintError
 *
 * Print and pop error.
 *
 */
void JSEngine::PrintError() const
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
/*****************************************************************************/
/**
 * @brief PrintTop
 *
 * Prints the top value information of the stack
 */
void JSEngine::PrintTop() const
{
    std::cout << "Stack size: " << duk_get_top(mCtx) << std::endl;
    std::cout << "Top type is: " << duk_get_type(mCtx, -1) << std::endl;
}
