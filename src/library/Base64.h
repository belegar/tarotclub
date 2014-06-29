#ifndef BASE64_H
#define BASE64_H

#include <string>

class Base64
{

public:
    static std::string Encode(const std::string &bytes_to_encode);
    static std::string Decode(const std::string &encoded_string);

};

#endif // BASE64_H
