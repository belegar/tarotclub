/*=============================================================================
 * TarotClub - Sha1.h
 *=============================================================================
 * Sha1 hash C++ implementation
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
#ifndef SHA1_HPP
#define SHA1_HPP

#include <iostream>
#include <string>
#include <cstdint>

/*****************************************************************************/
/**
 * @brief Sha1
 * This implementation supports two output formats (ASCII or binary)
 */
class Sha1
{
public:
    Sha1();
    void Update(const std::string &s);
    void Update(std::istream &is);
	std::string Final(bool ascii);
	static std::string FromFile(const std::string &filename, bool ascii);

private:
    static const unsigned int DIGEST_INTS = 5;  /* number of 32bit integers per SHA1 digest */
    static const unsigned int BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
    static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

    std::uint32_t digest[DIGEST_INTS];
    std::string buffer;
    std::uint64_t transforms;

    void Reset();
    void Transform(std::uint32_t block[BLOCK_BYTES]);

    static void BufferToBlock(const std::string &buffer, std::uint32_t block[BLOCK_BYTES]);
    static void Read(std::istream &is, std::string &s, int max);
};


#endif  // SHA1_HPP

//=============================================================================
// End of file Sha1.h
//=============================================================================

