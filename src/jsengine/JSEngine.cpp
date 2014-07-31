/*=============================================================================
 * TarotClub - JSEngine.cpp
 *=============================================================================
 * JavaScript engine class
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#include "JSEngine.h"
#include <fstream>

/*****************************************************************************/
JSEngine::JSEngine()
    : mCtx(NULL)
    , mValidContext(false)
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
    Close();
    mCtx = duk_create_heap_default();
    if (mCtx != NULL)
    {
        mValidContext = true;
    }
}
/*****************************************************************************/
bool JSEngine::Evaluate(const std::string &fileName)
{
    if (!mValidContext)
    {
        return false;
    }

    // Test if the file exists, try to open it!
    std::ifstream is;
    is.open(fileName, std::ios_base::in);
    if (!is.is_open())
    {
        // file does not exists
        return false;
    }
    is.close();

    // Push argument into the stack: file name to evaluate
    duk_push_lstring(mCtx, fileName.c_str(), fileName.size());

    int rc = duk_safe_call(mCtx, JSEngine::WrappedScriptEval, 1 /*nargs*/, 0 /*nrets*/);
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
Value JSEngine::Call(const std::string &function, const IScriptEngine::StringList &args)
{
    Value retval; // default is invalid value

    if (!mValidContext)
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

        int rc = duk_safe_call(mCtx, JSEngine::WrappedScriptCall, 1 /*nargs*/, 1 /*nrets*/);
        if (rc != DUK_EXEC_SUCCESS)
        {
            PrintError();
        }
        else
        {
            if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
            {
                std::string value = duk_get_string(mCtx, -1);
                retval = Value(value);
            }
            if (duk_check_type(mCtx, -1, DUK_TYPE_BOOLEAN))
            {
                bool value = false;
                if (duk_get_boolean(mCtx, -1))
                {
                    value = true;
                }
                retval = Value(value);
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
    if (mCtx != NULL)
    {
        duk_destroy_heap(mCtx);
        mCtx = NULL;
    }
    mValidContext = false;
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

//=============================================================================
// End of file JSEngine.cpp
//=============================================================================
