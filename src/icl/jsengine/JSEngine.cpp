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
#include "Log.h"
#include <fstream>
#include <sstream>

#define DUKTAPE_DEBUG

static const char *gIdName = "ctx_id";
static std::mutex gListenersMutex;
static std::uint32_t gIdCounter = 1U; // global counter to deliver an ID to each script instance
static std::map<std::uint32_t, IScriptEngine::IPrinter*> gPrintList;

static duk_ret_t SystemPrint(duk_context *ctx)
{
    std::string msg;
    duk_idx_t nargs = duk_get_top(ctx);

    /* If argument count is 1 and first argument is a buffer, write the buffer
     * as raw data into the file without a newline; this allows exact control
     * over stdout/stderr without an additional entrypoint (useful for now).
     * Otherwise current print/alert semantics are to ToString() coerce
     * arguments, join them with a single space, and append a newline.
     */

    if (nargs == 1 && duk_is_buffer(ctx, 0))
    {
        duk_size_t sz_buf;
        const char *buf = (const char *) duk_get_buffer(ctx, 0, &sz_buf);
        msg.assign(buf, sz_buf);
    }
    else
    {
        duk_push_string(ctx, " ");
        duk_insert(ctx, 0);
        duk_concat(ctx, nargs);
        msg = std::string(duk_require_string(ctx, -1));
    }

    msg += "\r\n";

    // Get the instance id if that context
    duk_bool_t ok = duk_get_global_string(ctx, gIdName);
    if (ok != 0U)
    {
        std::uint32_t id = duk_get_uint(ctx, -1);

        gListenersMutex.lock();
        // Call the listener of that id, if it exists
        if (gPrintList.count(id) > 0U)
        {
            gPrintList[id]->Print(msg);
        }
        gListenersMutex.unlock();
    }

    return 0;
}

static void fatal_handler(void *udata, const char *msg)
{
    (void) udata;
    TLogError("JS engine fatal error: " + std::string(msg));
}

static duk_ret_t tostring_raw(duk_context *ctx, void *udata)
{
    (void) udata;
    duk_to_string(ctx, -1);
    return 1;
}

static int eval_string_raw(duk_context *ctx, void *udata)
{
    (void) udata;

    duk_eval(ctx);
    return 1;
}


/*****************************************************************************/
JSEngine::JSEngine()
    : mCtx(NULL)
    , mValidContext(false)
    , mId(0U)
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
    mCtx = duk_create_heap(NULL /*duk_alloc_function alloc_func*/,
                           NULL/*duk_realloc_function realloc_func*/,
                           NULL/*duk_free_function free_func*/,
                           NULL/*void *heap_udata*/,
                           fatal_handler);
    if (mCtx != NULL)
    {
        // Register our custom print function
        duk_push_global_object(mCtx);
        duk_push_c_function(mCtx, SystemPrint, DUK_VARARGS);
        duk_put_prop_string(mCtx, -2 /*idx:global*/, "systemPrint");
        duk_pop(mCtx);  /* pop global */

        // Generate an id for that context
        gListenersMutex.lock();
        mId = gIdCounter;
        gIdCounter++;
        gListenersMutex.unlock();

        // Register our context ID
        duk_push_uint(mCtx, mId);
        (void) duk_put_global_string(mCtx, "ctx_id");

        mValidContext = true;
    }
    else
    {
        TLogScript("Cannot initialize script context");
    }
}
/*****************************************************************************/
void JSEngine::RegisterPrinter(IScriptEngine::IPrinter *printer)
{
    if (printer != nullptr)
    {
        gListenersMutex.lock();
        gPrintList[mId] = printer;
        gListenersMutex.unlock();
    }
}
/*****************************************************************************/
bool JSEngine::EvaluateFile(const std::string &fileName)
{
    bool ret = false;
    if (mValidContext)
    {
        // Test if the file exists, try to open it!
        std::ifstream in;
        in.open(fileName, std::ios_base::in | std::ios::binary);
        if (in.is_open())
        {
            std::string contents;
            std::string output;

            // Read whole file and store it into memory
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();

            ret = EvaluateString(contents, output);
        }
    }

    return ret;
}
/*****************************************************************************/
bool JSEngine::EvaluateString(const std::string &contents, std::string &output)
{
    bool ret = false;
    if (mValidContext)
    {
        duk_push_string(mCtx, contents.c_str());
        int rc = duk_safe_call(mCtx, eval_string_raw, NULL /*udata*/, 1 /*nargs*/, 1 /*nrets*/);
        (void) duk_safe_call(mCtx, tostring_raw /*udata*/, NULL, 1 /*nargs*/, 1 /*nrets*/);
        output = duk_get_string(mCtx, -1);

        if (rc == DUK_EXEC_SUCCESS)
        {
            ret = true;
        }
        else
        {
            output = "Compile failed: " + output;
        }
    }
    return ret;
}
/*****************************************************************************/
Value JSEngine::Call(const std::string &function, const IScriptEngine::StringList &args)
{
    Value retval; // default is invalid value

    if (!mValidContext)
    {
        TLogError("Cannot call script function without a valid context.");
        return retval;
    }

    // Push the Ecmascript global object to the value stack
    duk_push_global_object(mCtx);
    duk_get_prop_string(mCtx, -1 /*index*/, function.c_str()); // actually gets this function name

#ifdef DUKTAPE_DEBUG
  //  PrintTop();
#endif

    if (!duk_is_undefined(mCtx, -1))
    {
        // Push all the arguments
        for (size_t i = 0; i < args.size(); i++)
        {
            duk_push_string(mCtx, args[i].c_str());
        }

        int rc = duk_pcall(mCtx, args.size() /*nargs*/);
        if (rc != DUK_EXEC_SUCCESS)
        {
            TLogError(std::string("JS engine script call failed for function: ") + function);
            PrintError();
        }
        else
        {
            if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
            {
                std::string value = duk_get_string(mCtx, -1);
                retval = Value(value);
            }
            else if (duk_check_type(mCtx, -1, DUK_TYPE_BOOLEAN))
            {
                bool value = false;
                if (duk_get_boolean(mCtx, -1))
                {
                    value = true;
                }
                retval = Value(value);
            }
            else
            {
                duk_int_t ret = duk_get_type(mCtx, -1);

                if (ret != DUK_TYPE_UNDEFINED)
                {
                    std::stringstream ss;
                    ss << "Unsupported value type returned: " << ret;
                    TLogError(ss.str());
                }
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
/**
 * @brief PrintError
 *
 * Print and pop error.
 *
 */
void JSEngine::PrintError() const
{

#ifdef DUKTAPE_DEBUG
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
