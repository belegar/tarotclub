/*=============================================================================
 * TarotClub - ByteStreamWriter.h
 *=============================================================================
 * Utility class to write to a ByteArray in a stream fashion
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

#ifndef BYTE_STREAM_WRITER_H
#define BYTE_STREAM_WRITER_H

#include <deque>
#include <cstdint>
#include <iostream>
#include "ByteArray.h"

/*****************************************************************************/
class ByteStreamWriter
{

public:
    ByteStreamWriter(ByteArray &array);

    /**
     * @brief Sets the write/read index to a specified position in the array
     * @param pos
     */
    void Seek(std::uint32_t pos);

    // Operators
    ByteStreamWriter &operator << (const std::uint8_t &d);
    ByteStreamWriter &operator << (const std::uint16_t &d);
    ByteStreamWriter &operator << (const std::uint32_t &d);
    ByteStreamWriter &operator << (const std::int32_t &d);
    ByteStreamWriter &operator << (const bool &d);
    /**
     * @brief Overloaded operator to add a string to a byte stream
     * The first uint32_t will contain the size of the string
     * Then the string bytes are streamed, without any null-termination
     * @param s
     * @return
     */
    ByteStreamWriter &operator << (const std::string &s);

private:
    ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTE_STREAM_WRITER_H

//=============================================================================
// End of file ByteStreamWriter.h
//=============================================================================
