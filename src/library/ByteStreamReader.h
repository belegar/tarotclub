#ifndef BYTE_STREAM_READER_H
#define BYTE_STREAM_READER_H

#include <deque>
#include <cstdint>
#include <sstream>
#include "ByteArray.h"

class ByteStreamReader
{

public:
    ByteStreamReader(const ByteArray &array)
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

    std::string ToString()
    {
        std::stringstream array;
        array << "Array: ";
        for (std::uint32_t i = 0; i < mArray.Size(); i++)
        {
            array << std::hex << (std::uint32_t)mArray[i] << ", ";
        }
        array << std::endl;

        return array.str();
    }

    ByteStreamReader& operator >> (std::uint8_t &d)
    {
        if (mIndex < mArray.Size())
        {
            d = mArray[mIndex];
            mIndex++;
        }
        return *this;
    }

    ByteStreamReader& operator >> (std::uint16_t &d)
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

    ByteStreamReader& operator >> (std::uint32_t &d)
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

    ByteStreamReader& operator >> (bool &d)
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

    ByteStreamReader& operator >> (std::string &s)
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

private:
    const ByteArray &mArray;
    std::uint32_t mIndex;
};

#endif // BYTE_STREAM_READER_H
