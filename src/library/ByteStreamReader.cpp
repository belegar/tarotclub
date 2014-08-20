/*=============================================================================
 * TarotClub - ByteStreamReader.cpp
 *=============================================================================
 * Utility class to read from a ByteArray in a stream fashion
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

#include <deque>
#include <cstdint>
#include <sstream>
#include "ByteStreamReader.h"

/*****************************************************************************/
ByteStreamReader::ByteStreamReader(const ByteArray &array)
    : mArray(array)
    , mIndex(0U)
{

}
/*****************************************************************************/
/**
 * @brief Sets the write/read index to a specified position in the array
 * @param pos
 */
void ByteStreamReader::Seek(std::uint32_t pos)
{
    if (pos < mArray.Size())
    {
        mIndex = pos;
    }
    else
    {
        mIndex = mArray.Size();
    }
}
/*****************************************************************************/
std::string ByteStreamReader::ToString()
{
    std::stringstream array;
    array << "Array: ";
    for (std::uint32_t i = 0; i < mArray.Size(); i++)
    {
        array << std::hex << (std::uint32_t)mArray.Get(i) << ", ";
    }
    array << std::endl;

    return array.str();
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint8_t &d)
{
    if (mIndex < mArray.Size())
    {
        d = mArray.Get(mIndex);
        mIndex++;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint16_t &d)
{
    std::uint8_t byte;
    std::uint16_t hword;

    d = 0U;
    if (mArray.Size() >= 2U)
    {
        for (std::uint8_t i = 0U; i < 2U; i++)
        {
            *this >> byte;
            hword = byte;
            d = d + (hword << (i * 8U));
        }
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::uint32_t &d)
{
    std::uint8_t byte;
    std::uint32_t word;

    d = 0U;
    if (mArray.Size() >= 4U)
    {
        for (std::uint8_t i = 0U; i < 4U; i++)
        {
            *this >> byte;
            word = byte;
            d = d + (word << (i * 8U));
        }
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (bool &d)
{
    std::uint8_t byte;

    *this >> byte;
    if (byte)
    {
        d = true;
    }
    else
    {
        d = false;
    }
    return *this;
}
/*****************************************************************************/
ByteStreamReader &ByteStreamReader::operator >> (std::string &s)
{
    std::uint32_t size;
    std::uint8_t byte;

    *this >> size;
    for (std::uint32_t i = 0U; i < size; i++)
    {
        *this >> byte;
        s.push_back(static_cast<char>(byte));
    }
    return *this;
}

//=============================================================================
// End of file ByteStreamReader.cpp
//=============================================================================
