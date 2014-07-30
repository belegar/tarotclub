/*=============================================================================
 * TarotClub - Sha1.cpp
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

#include "Sha1.h"
#include <sstream>
#include <iomanip>
#include <fstream>

/*****************************************************************************/
Sha1::Sha1()
{
    Reset();
}

/*****************************************************************************/
void Sha1::Update(const std::string &s)
{
    std::istringstream is(s);
    Update(is);
}

/*****************************************************************************/
void Sha1::Update(std::istream &is)
{
    std::string rest_of_buffer;
    Read(is, rest_of_buffer, BLOCK_BYTES - buffer.size());
    buffer += rest_of_buffer;

    while (is)
    {
        std::uint32_t block[BLOCK_INTS];
        BufferToBlock(buffer, block);
        Transform(block);
        Read(is, buffer, BLOCK_BYTES);
    }
}

/*****************************************************************************/
/*
 * Add padding and return the message digest.
 */

std::string Sha1::Final(bool ascii)
{
	/* Total number of hashed bits */
	std::uint64_t total_bits = (transforms * BLOCK_BYTES + buffer.size()) * 8;

	/* Padding */
	buffer += (char)0x80;
	size_t orig_size = buffer.size();
	while (buffer.size() < BLOCK_BYTES)
	{
		buffer += (char)0x00;
	}

	std::uint32_t block[BLOCK_INTS];
	BufferToBlock(buffer, block);

	if (orig_size > BLOCK_BYTES - 8)
	{
		Transform(block);
		for (unsigned int i = 0; i < BLOCK_INTS - 2; i++)
		{
			block[i] = 0;
		}
	}

	/* Append total_bits, split this std::uint64_t into two std::uint32_t */
	block[BLOCK_INTS - 1] = static_cast<std::uint32_t>(total_bits);
	block[BLOCK_INTS - 2] = (total_bits >> 32);
	Transform(block);

	/* Hex std::string */
	std::string output;

	if (ascii)
	{
		std::ostringstream result;
		for (unsigned int i = 0; i < DIGEST_INTS; i++)
		{
			result << std::hex << std::setfill('0') << std::setw(8);
			result << (digest[i] & 0xffffffff);
		}
		output = result.str();
	}
	else
	{
		for (unsigned int i = 0; i < DIGEST_INTS; i++)
		{
			std::uint8_t byte;
			std::uint32_t mask = 0xFF000000U;
			std::uint32_t pos = 24U;
			std::uint32_t data = digest[i];

			for (std::uint8_t j = 0U; j < 4U; j++)
			{
				byte = (data & mask) >> pos;
				output.push_back(byte);
				pos -= 8;
				mask >>= 8;
			}
		}
	}

    /* Reset for next run */
    Reset();

    return output;
}

/*****************************************************************************/
std::string Sha1::FromFile(const std::string &filename, bool ascii)
{
    std::ifstream stream(filename.c_str(), std::ios::binary);
    Sha1 checksum;
    checksum.Update(stream);
    return checksum.Final(ascii);
}

/*****************************************************************************/
void Sha1::Reset()
{
    /* Sha1 initialization constants */
    digest[0] = 0x67452301;
    digest[1] = 0xefcdab89;
    digest[2] = 0x98badcfe;
    digest[3] = 0x10325476;
    digest[4] = 0xc3d2e1f0;

    /* Reset counters */
    transforms = 0;
    buffer = "";
}

/*****************************************************************************/
/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */

void Sha1::Transform(std::uint32_t block[])
{
    /* Copy digest[] to working vars */
    std::uint32_t a = digest[0];
    std::uint32_t b = digest[1];
    std::uint32_t c = digest[2];
    std::uint32_t d = digest[3];
    std::uint32_t e = digest[4];

    /* Help macros */
#define rol(value, bits) (((value) << (bits)) | (((value) & 0xffffffff) >> (32 - (bits))))
#define blk(i) (block[i&15] = rol(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i&15],1))

    /* (R0+R1), R2, R3, R4 are the different operations used in Sha1 */
#define R0(v,w,x,y,z,i) z += ((w&(x^y))^y)     + block[i] + 0x5a827999 + rol(v,5); w=rol(w,30);
#define R1(v,w,x,y,z,i) z += ((w&(x^y))^y)     + blk(i)   + 0x5a827999 + rol(v,5); w=rol(w,30);
#define R2(v,w,x,y,z,i) z += (w^x^y)           + blk(i)   + 0x6ed9eba1 + rol(v,5); w=rol(w,30);
#define R3(v,w,x,y,z,i) z += (((w|x)&y)|(w&x)) + blk(i)   + 0x8f1bbcdc + rol(v,5); w=rol(w,30);
#define R4(v,w,x,y,z,i) z += (w^x^y)           + blk(i)   + 0xca62c1d6 + rol(v,5); w=rol(w,30);

    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);

    /* Add the working vars back into digest[] */
    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;

    /* Count the number of transformations */
    transforms++;
}
/*****************************************************************************/
void Sha1::BufferToBlock(const std::string &buffer, std::uint32_t block[])
{
    /* Convert the std::string (byte buffer) to a std::uint32_t array (MSB) */
    for (unsigned int i = 0; i < BLOCK_INTS; i++)
    {
        block[i] = (buffer[4 * i + 3] & 0xff)
                   | (buffer[4 * i + 2] & 0xff) << 8
                   | (buffer[4 * i + 1] & 0xff) << 16
                   | (buffer[4 * i + 0] & 0xff) << 24;
    }
}
/*****************************************************************************/
void Sha1::Read(std::istream &is, std::string &s, int max)
{
    char *sbuf = new char[max];
    is.read(sbuf, max);
    s.assign(sbuf, static_cast<unsigned>(is.gcount()));
	delete[] sbuf;
}

//=============================================================================
// End of file Sha1.cpp
//=============================================================================
