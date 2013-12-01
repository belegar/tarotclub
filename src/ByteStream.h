#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <deque>
#include <cstdint>
#include <iostream>
#include "ByteArray.h"

class ByteStream
{

public:
    ByteStream(ByteArray &array)
        : mArray(array)
        , mIndex(0U)
    {

    }

    /**
     * @brief Sets the write/read index to a specified position in the array
     * @param pos
     */
    void Seek(std::uint32_t pos)
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

    void Print()
    {
        std::cout << "Array: ";
        for (std::uint32_t i = 0; i < mArray.Size(); i++)
        {
            std::cout << std::hex << (std::uint32_t)mArray[i] << ", ";
        }
        std::cout << std::endl;
    }

    ByteStream& operator >> (std::uint8_t &d)
    {
        if (mArray.Size() >= 1U)
        {
            d = mArray.PopFront();
        }

        // Adjust index if out of limits
        if (mIndex >= mArray.Size())
        {
            if (mArray.Size())
            {
                mIndex = mArray.Size() - 1U;
            }
            else
            {
                mIndex = 0U;
            }
        }
        return *this;
    }

    ByteStream& operator >> (std::uint16_t &d)
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
                d = d + (hword << (i*8U));
            }
        }
        return *this;
    }

    ByteStream& operator >> (std::uint32_t &d)
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
                d = d + (word << (i*8U));
            }
        }
        return *this;
    }

    ByteStream& operator >> (std::string &s)
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

    //-----------------------------------------------------------------------

    ByteStream& operator << (const std::uint8_t &d)
    {
        if (mIndex >= mArray.Size())
        {
            // Append data at the end
            mArray.PushBack(d);
        }
        else
        {
            // Overwrite data
            mArray[mIndex] = d;
        }
        mIndex++;
        return *this;
    }

    ByteStream& operator << (const std::uint16_t &d)
    {
        std::uint8_t byte;
        std::uint16_t data = d;

        for (std::uint8_t i = 0U; i < 2U; i++)
        {
            byte = data & 0xFF;
            *this << byte;
            data = data >> 8;
        }
        return *this;
    }

    ByteStream& operator << (std::uint32_t &d)
    {
        std::uint8_t byte;
        std::uint32_t data = d;

        for (std::uint8_t i = 0U; i < 4U; i++)
        {
            byte = data & 0xFF;
            *this << byte;
            data = data >> 8;
        }
        return *this;
    }

    /**
     * @brief Overloaded operator to add a string to a byte stream
     * The first uint32_t will contain the size of the string
     * Then the string bytes are streamed, without any null-termination
     * @param s
     * @return
     */
    ByteStream& operator << (const std::string &s)
    {
        std::uint32_t size = s.size();
        std::uint8_t byte;

        *this << size;
        for (std::uint32_t i = 0U; i < size; i++)
        {
            byte = static_cast<std::uint8_t>(s.at(i));
            *this << byte;
        }
        return *this;
    }

private:
    ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTESTREAM_H
