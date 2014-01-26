#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "JSValue.h"

/*****************************************************************************/
class IJsonNode
{
public:
    virtual ~IJsonNode() {}

    virtual std::string ToString() = 0;
};
/*****************************************************************************/
class JsonValue : public JSValue, public IJsonNode
{
public:
    JsonValue(std::int32_t value) : JSValue(value) {}
    JsonValue(double value) : JSValue(value) {}
    JsonValue(const char *value) : JSValue(value) {}
    JsonValue(const std::string &value) : JSValue(value) {}
    JsonValue(bool value) : JSValue(value) {}
    JsonValue(const JSValue &value) : JSValue(value) {}
    JsonValue() : JSValue() {}

    std::string ToString();
};

#endif // JSONVALUE_H
