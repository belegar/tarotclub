/*=============================================================================
 * TarotClub - ByteStreamReader.h
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

#ifndef BYTE_STREAM_READER_H
#define BYTE_STREAM_READER_H

#include <deque>
#include <cstdint>
#include <sstream>
#include "ByteArray.h"

/*****************************************************************************/
class ByteStreamReader
{

public:
    ByteStreamReader(const ByteArray &array);

    /**
     * @brief Sets the write/read index to a specified position in the array
     * @param pos
     */
    void Seek(std::uint32_t pos);
    std::string ToString();

    // Operators
    ByteStreamReader &operator >> (std::uint8_t &d);
    ByteStreamReader &operator >> (std::uint16_t &d);
    ByteStreamReader &operator >> (std::uint32_t &d);
    ByteStreamReader &operator >> (std::int32_t &d);
    ByteStreamReader &operator >> (bool &d);
    ByteStreamReader &operator >> (std::string &s);

private:
    const ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTE_STREAM_READER_H

//=============================================================================
// End of file ByteStreamReader.h
//=============================================================================
