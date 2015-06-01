/*=============================================================================
 * TarotClub - ByteArray.h
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

#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <vector>
#include <cstdint>
#include <iostream>

/*****************************************************************************/
/**
 * @brief The ByteArray class
 * Little endian
 *
 */
class ByteArray
{

public:

    // Constructors
    ByteArray(const char *data, std::uint32_t size);
    ByteArray(const std::string &data);
    ByteArray();

    // Getters
    std::uint16_t GetUint16(std::uint32_t index) const;
    std::uint32_t GetUint32(std::uint32_t index) const;
    std::string ToSring() const;
    std::uint32_t Size() const;
    std::uint8_t Get(std::uint32_t index) const;

    // Setters
    void PushBack(std::uint8_t byte);
    void Put(std::uint32_t index, std::uint8_t value);

    // Helpers
    void Clear();
    ByteArray SubArray(std::uint32_t index, std::uint32_t len) const;
    void Erase(std::uint32_t index, std::uint32_t len);
    void Alloc(std::uint32_t size);
    std::uint8_t *Data();
    int32_t FindFirstOf(std::uint8_t item);

    // Operators
    bool operator == (const std::string &rhs) const;
    ByteArray &operator=(const ByteArray &rhs);
    ByteArray &operator += (const ByteArray &rhs);
    ByteArray operator+(const ByteArray &rhs) const;

private:
    std::vector<std::uint8_t> mData;

    void Append(const std::vector<std::uint8_t> &data);
};


#endif // BYTEARRAY_H

//=============================================================================
// End of file ByteArray.h
//=============================================================================
