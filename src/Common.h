#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <cstdint>
#include <vector>
#include "ByteStreamReader.h"
#include "ByteStreamWriter.h"

/*****************************************************************************/
class Place
{
public:
    static const std::string STR_SOUTH;
    static const std::string STR_EAST;
    static const std::string STR_NORTH;
    static const std::string STR_WEST;

    static const std::uint8_t SOUTH;
    static const std::uint8_t EAST;
    static const std::uint8_t NORTH;
    static const std::uint8_t WEST;
    static const std::uint8_t FIFTH;
    static const std::uint8_t NOWHERE;

    // Constructors
    Place();
    Place(int p);
    Place(std::uint8_t p);

    std::string ToString() const;
    std::uint8_t Value();

    Place &operator = (Place const &rhs)
    {
        mPlace = rhs.mPlace;
        return *this;
    }

    inline bool operator == (const Place& rhs) const { return (this->mPlace == rhs.mPlace); }
    inline bool operator != (const Place& rhs) const { return (this->mPlace != rhs.mPlace); }
    inline bool operator <  (const Place& rhs) const { return (this->mPlace < rhs.mPlace);  }
    inline bool operator >  (const Place& rhs) const { return (this->mPlace > rhs.mPlace); }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Place &p)
    {
        out << p.mPlace;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Place &l)
    {
        s >> l.mPlace;
        return s;
    }

private:
    std::uint8_t mPlace;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};
/*****************************************************************************/
class Contract
{
public:
    static const std::string STR_PASS;
    static const std::string STR_TAKE;
    static const std::string STR_GUARD;
    static const std::string STR_GUARD_WITHOUT;
    static const std::string STR_GUARD_AGAINST;

    static const std::uint8_t PASS;
    static const std::uint8_t TAKE;
    static const std::uint8_t GUARD;
    static const std::uint8_t GUARD_WITHOUT;
    static const std::uint8_t GUARD_AGAINST;

    // Constructors
    Contract();
    Contract(int c);
    Contract(std::uint8_t c);

    std::string ToString() const;
    std::uint8_t Value();

    Contract &operator = (Contract const &rhs)
    {
        mContract = rhs.mContract;
        return *this;
    }

    inline bool operator == (const Contract& rhs) const { return (this->mContract == rhs.mContract);}
    inline bool operator != (const Contract& rhs) const { return (this->mContract != rhs.mContract);}
    inline bool operator <  (const Contract& rhs) const { return (this->mContract < rhs.mContract); }
    inline bool operator <= (const Contract& rhs) const { return (this->mContract <= rhs.mContract);}
    inline bool operator >  (const Contract& rhs) const { return (this->mContract > rhs.mContract); }
    inline bool operator >= (const Contract& rhs) const { return (this->mContract >=rhs.mContract); }

    friend ByteStreamWriter &operator<<(ByteStreamWriter &out, const Contract &p)
    {
        out << p.mContract;
        return out;
    }

    friend ByteStreamReader &operator>>(ByteStreamReader &s, Contract &l)
    {
        s >> l.mContract;
        return s;
    }

private:
    std::uint8_t mContract;
    static std::vector<std::string> mStrings;

    static std::vector<std::string> Initialize();
};


#endif // COMMON_H
