
#ifndef JSON_READER_H
#define JSON_READER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "duktape.h"

class JsonReader
{

public:
    JsonReader()
    {
        mCtx = NULL;
    }
    
    ~JsonReader()
    {
        Close();
    }
    
    /**
     * @brief Open a JSON file for parsing
     *
     * After opening the JSON file, the context is ready for requests.
     *
     * @param fileName to open
     * @return true if the file has been parsed successfully
     */
    bool Open(const std::string &fileName);


    /**
     * @brief GetValue
     *
     * Gets a JSON value by specifying the key.
     * The value returned must be a string format, if not, false is returned
     *
     * @param key
     * @param value
     * @return
     */
    bool GetValue(const std::string &key, std::string &value);

    bool GetValue(const std::string &obj, const std::string &key, std::string &value);

    /**
     * @brief Close
     *
     * Cleans the context and free memory
     *
     */
    void Close();
    
private:
    duk_context *mCtx;
    bool mValid;

    /**
     * @brief WrappedJsonDecode
     *
     * Decode a JSON string, throw an error if parsing problem
     *
     * @param ctx
     * @return
     */
    static int WrappedJsonDecode(duk_context *ctx);

    /**
     * @brief WrappedJsonGetValue
     *
     * Key is pushed into the stack
     *
     * @param ctx
     * @return
     */
    static int WrappedJsonGetValue(duk_context *ctx);

    /**
     * @brief WrappedJsonGetObjectValue
     *
     * Gets a property in an object value
     *
     * @param ctx
     * @return
     */
    static int WrappedJsonGetObjectValue(duk_context *ctx);

    /**
     * @brief PrintError
     *
     * Print and pop error.
     *
     */
    void PrintError();

    /**
     * @brief PrintTop
     *
     * Prints the top value information of the stack
     */
    void PrintTop();
};

#endif // JSON_READER_H
