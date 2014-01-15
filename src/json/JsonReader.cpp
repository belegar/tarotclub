
#include "JsonReader.h"

/*****************************************************************************/
bool JsonReader::Open(const std::string &fileName)
{
    std::ifstream f;
    bool retval = false;
    f.open(fileName, std::ios_base::in | std::ios_base::binary);

    if (f.is_open())
    {
        mCtx = duk_create_heap_default();
        std::ostringstream contents;
        contents << f.rdbuf();
        f.close();

        duk_push_lstring(mCtx, contents.str().c_str(), contents.str().size());

        int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonDecode, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
        if (rc != DUK_EXEC_SUCCESS)
        {
            PrintError();
            mValid = false;
            retval = false;
        }
        else
        {
            mValid = true;
            retval = true;
        }
    }

    return retval;
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &key, std::string &value)
{
    if (!mValid)
    {
        return false;
    }

    // Push argument into the stack (JSON key)
    duk_push_lstring(mCtx, key.c_str(), key.size());

    int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonGetValue, 1 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
    if (rc != DUK_EXEC_SUCCESS)
    {
        PrintError();
        return false;
    }
    else
    {
        if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
        {
            value = duk_get_string(mCtx, -1);
            duk_pop(mCtx);
        }
        return true;
    }
}
/*****************************************************************************/
bool JsonReader::GetValue(const std::string &obj, const std::string &key, std::string &value)
{
    if (!mValid)
    {
        return false;
    }

    // Push argument into the stack (JSON key)
    duk_push_lstring(mCtx, obj.c_str(), obj.size());
    duk_push_lstring(mCtx, key.c_str(), key.size());

    int rc = duk_safe_call(mCtx, JsonReader::WrappedJsonGetObjectValue, 2 /*nargs*/, 1 /*nret*/, DUK_INVALID_INDEX);
    if (rc != DUK_EXEC_SUCCESS)
    {
        PrintError();
        return false;
    }
    else
    {
        PrintTop();
        if (duk_check_type(mCtx, -1, DUK_TYPE_STRING))
        {
            value = duk_get_string(mCtx, -1);
            duk_pop(mCtx);
        }
        return true;
    }

}
/*****************************************************************************/
void JsonReader::Close()
{
    if (mCtx)
    {
        duk_destroy_heap(mCtx);
    }
    mValid = false;
}
/*****************************************************************************/
void JsonReader::PrintError()
{
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
}
/*****************************************************************************/
int JsonReader::WrappedJsonDecode(duk_context *ctx)
{
    duk_json_decode(ctx, -1);
    return 1; // return parsed json object
}
/*****************************************************************************/
int JsonReader::WrappedJsonGetValue(duk_context *ctx)
{
    std::string key = duk_get_string(ctx, -1);
    duk_pop(ctx);

    if(duk_has_prop_string(ctx, -1, key.c_str()))
    {
        duk_get_prop_string(ctx, -1, key.c_str());
        // leave the answer into the stack to retreive it by the caller
        // so, we have one return value
        return 1;
    }
    return 0; // no return values
}
/*****************************************************************************/
int JsonReader::WrappedJsonGetObjectValue(duk_context *ctx)
{
    std::string obj = duk_get_string(ctx, -2); // [ json "obj" "key" ]
    std::string key = duk_get_string(ctx, -1);
    duk_pop(ctx); // [ json "obj" ]

    std::cout << "Stack size: " << duk_get_top(ctx) << std::endl;
    std::cout << "Top type is: " << duk_get_type(ctx, -1) << std::endl;

    duk_get_prop(ctx, -2);  // [ json obj ] object name has been replaced by the object

    std::cout << "Stack size: " << duk_get_top(ctx) << std::endl;
    std::cout << "Top type is: " << duk_get_type(ctx, -1) << std::endl;

    if (duk_has_prop_string(ctx, -1, key.c_str()))
    {
        duk_get_prop_string(ctx, -1, key.c_str());
        // leave the answer into the stack to retreive it by the caller
        // so, we have one return value
        return 1;
    }
    return 0; // no return values
}
/*****************************************************************************/
void JsonReader::PrintTop()
{
    std::cout << "Stack size: " << duk_get_top(mCtx) << std::endl;
    std::cout << "Top type is: " << duk_get_type(mCtx, -1) << std::endl;
}

