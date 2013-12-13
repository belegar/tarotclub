#ifndef BYTE_STREAM_WRITER_H
#define BYTE_STREAM_WRITER_H

#include <deque>
#include <cstdint>
#include <iostream>
#include "ByteArray.h"

class ByteStreamWriter
{

public:
    ByteStreamWriter(ByteArray &array)
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

    ByteStreamWriter& operator << (const std::uint8_t &d)
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

    ByteStreamWriter& operator << (const std::uint16_t &d)
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

    ByteStreamWriter& operator << (const std::uint32_t &d)
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
    ByteStreamWriter& operator << (const std::string &s)
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

#endif // BYTE_STREAM_WRITER_H
