/*=============================================================================
 * TarotClub - ByteArray.cpp
 *=============================================================================
 * This utility class is a dynamic array of bytes
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

#include <vector>
#include <cstdint>
#include <iostream>
#include "ByteArray.h"

/*****************************************************************************/
ByteArray::ByteArray(const char *data, std::uint32_t size)
{
    for (std::uint32_t i = 0U; i < size; i++)
    {
        mData.push_back(static_cast<std::uint8_t>(data[i]));
    }
}
/*****************************************************************************/
ByteArray::ByteArray(const std::string &data)
{
    for (std::uint32_t i = 0U; i < data.size(); i++)
    {
        mData.push_back(static_cast<std::uint8_t>(data[i]));
    }
}
/*****************************************************************************/
ByteArray::ByteArray()
{

}
/*****************************************************************************/
void ByteArray::PushBack(std::uint8_t byte)
{
    mData.push_back(byte);
}
/*****************************************************************************/
std::uint32_t ByteArray::Size() const
{
    return mData.size();
}
/*****************************************************************************/
void ByteArray::Clear()
{
    mData.clear();
}
/*****************************************************************************/
ByteArray ByteArray::SubArray(std::uint32_t index, std::uint32_t len)
{
    ByteArray array;

    if ((index + len) <= mData.size())
    {
        array.mData.insert(array.mData.begin(), mData.begin() + index, mData.begin() + index + len);
    }
    return array;
}
/*****************************************************************************/
void ByteArray::Erase(std::uint32_t index, std::uint32_t len)
{
    mData.erase(mData.begin() + index, mData.begin() + index + len);
}
/*****************************************************************************/
std::uint8_t& ByteArray::operator[] (std::uint32_t i)
{
    if (i >= mData.size())
    {
        return mData[0];
    }
    else
    {
        return mData[i];
    }
}
/*****************************************************************************/
std::uint8_t ByteArray::operator[] (std::uint32_t i) const
{
    if (i >= mData.size())
    {
        return mData[0];
    }
    else
    {
        return mData[i];
    }
}
/*****************************************************************************/
ByteArray& ByteArray::operator=(const ByteArray &rhs)
{
  mData.clear();
  mData = rhs.mData;
  return *this;
}
/*****************************************************************************/
ByteArray& ByteArray::operator += (const ByteArray &rhs)
{
    *this = *this + rhs;
    return *this;
}
/*****************************************************************************/
ByteArray ByteArray::operator+(const ByteArray &rhs) const
{
    ByteArray result;
    result.Append(this->mData);
    result.Append(rhs.mData);
    return result;
}
/*****************************************************************************/
std::uint16_t ByteArray::GetUint16 (std::uint32_t index) const
{
    std::uint16_t data = 0U;

    if (mData.size() >= sizeof(std::uint16_t))
    {
        if (index <= (mData.size() - sizeof(std::uint16_t)))
        {
            for (std::uint8_t i = 0U; i < 2U; i++)
            {
                std::uint16_t hword = mData[index + i];
                data = data + (hword << (i*8U));
            }
        }
    }
    return data;
}
/*****************************************************************************/
std::uint32_t ByteArray::GetUint32 (std::uint32_t index) const
{
    std::uint32_t data = 0U;

    if (mData.size() >= sizeof(std::uint32_t))
    {
        if (index <= (mData.size() - sizeof(std::uint32_t)))
        {
            for (std::uint8_t i = 0U; i < 2U; i++)
            {
                std::uint32_t word = mData[index + i];
                data = data + (word << (i * 8U));
            }
        }
    }
    return data;
}
/*****************************************************************************/
std::string ByteArray::ToSring() const
{
    std::string buf;

    for (std::uint32_t i = 0U; i < mData.size(); i++)
    {
        buf.push_back(static_cast<char>(mData[i]));
    }

    return buf;
}
/*****************************************************************************/
void ByteArray::Append (const std::vector<std::uint8_t> &data)
{
    std::vector<std::uint8_t>::iterator it = mData.end();
    mData.insert(it, data.begin(), data.end());
}

//=============================================================================
// End of file ByteArray.cpp
//=============================================================================
