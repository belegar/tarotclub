#include "JSValue.h"

/*****************************************************************************/
JSValue::JSValue(std::int32_t value)
    : mType(INTEGER)
    , mIntegerValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(double value)
    : mType(DOUBLE)
    , mDoubleValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const char * value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const std::string &value)
    : mType(STRING)
    , mStringValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(bool value)
    : mType(BOOLEAN)
    , mBoolValue(value)
{

}
/*****************************************************************************/
JSValue::JSValue(const JSValue &value)
{
    *this = value;
}
/*****************************************************************************/
JSValue::JSValue()
{
    mType = INVALID;
}
/*****************************************************************************/
JSValue::~JSValue()
{

}
/*****************************************************************************/
JSValue &JSValue::operator =(const JSValue &rhs)
{
    mType = rhs.mType;
    mIntegerValue = rhs.mIntegerValue;
    mDoubleValue = rhs.mDoubleValue;
    mStringValue = rhs.mStringValue;
    mBoolValue = rhs.mBoolValue;
    return *this;
}
