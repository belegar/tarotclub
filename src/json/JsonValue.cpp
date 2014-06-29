#include "JsonValue.h"
#include <sstream>

/*****************************************************************************/
std::string JsonValue::ToString()
{
    std::string text;
    std::stringstream ss;

    if (GetType() == STRING)
    {
        text += "\"" + GetString() + "\"";
    }
    else if (GetType() == INTEGER)
    {
        ss << GetInteger();
        text += ss.str();
    }
    else if (GetType() == BOOLEAN)
    {
        if (GetBool())
        {
            text = "true";
        }
        else
        {
            text = "false";
        }
    }
    else if (GetType() == DOUBLE)
    {
        ss << GetDouble();
        text += ss.str();
    }

    return text;
}
