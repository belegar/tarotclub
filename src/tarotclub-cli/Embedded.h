/*=============================================================================
 * TarotClub - Embedded.h
 *=============================================================================
 * This file contains a file contents provided in an array
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

#include <cstdint>
#include <stdlib.h>
#include <assert.h>

/*****************************************************************************/
template<typename T>
class ArrayPtr
{
public:
    // STL-compatible iterator types
    typedef T* Iterator;
    typedef const T* ConstIterator;
	
	inline ArrayPtr() :
        mpData(NULL),
        mSize(0U)
    {
        // Nothing to do
    }

    inline ArrayPtr(const T* const i_pData, const std::uint32_t i_size) :
        mpData(i_pData),
        mSize(i_size)
    {
        // Nothing to do
    }

    inline ArrayPtr(const ArrayPtr<T>& i_array) :
        mpData(i_array.Data()),
        mSize(i_array.Size())
    {
        // Nothing to do
    }

    template<std::uint32_t Len>
    explicit inline ArrayPtr(const T (&i_array)[Len]) :
        mpData(&i_array[0]),
        mSize(Len)
    {
        // Nothing to do
    }

    // STL-compatible begin/end functions
    inline ConstIterator Begin() const
    {
        return mpData;
    }
    inline ConstIterator End() const
    {
        return (mpData == NULL ? mpData : &mpData[mSize]);
    }

    inline const T* Data() const
    {
        return mpData;
    }
    inline std::uint32_t Size() const
    {
        return mSize;
    }

    inline const T& operator [](const std::uint32_t i_index) const
    {
        assert(i_index < mSize);

        //lint -esym(1960,5-0-15) This class provides a wrapper around arrays. This is the only place where this
        // rule is permitted to be violated.
        //lint -e{613} It is a requirement that the ArrayPtr is non-null before calling this operator
        return mpData[i_index];
        //lint +esym(1960,5-0-15)
    }

    ArrayPtr<const T> SubArray(std::uint32_t i_index, std::uint32_t i_size) const
    {
        if (i_index > mSize)
        {
            i_index = mSize;
        }
        if (i_size > (mSize - i_index))
        {
            i_size = (mSize - i_index);
        }
        return ArrayPtr<const T>(&mpData[i_index], i_size);
    }

    ArrayPtr<const T> SubArray(std::uint32_t i_index) const
    {
        if (i_index > mSize)
        {
            i_index = mSize;
        }
        return ArrayPtr<const T>(&mpData[i_index], mSize - i_index);
    }

    void Reset()
    {
        mpData = NULL;
        mSize = 0U;
    }

    void Set(const T* const i_pArray, const std::uint32_t i_size)
    {
        mpData = i_pArray;
        mSize = i_size;
    }

    inline ArrayPtr<const T>& operator =(const ArrayPtr<T>& i_ptr)
    {
        mpData = i_ptr.Data();
        mSize = i_ptr.Size();
        return *this;
    }

    template<std::uint32_t Len>
    inline ArrayPtr<const T>& operator =(const T (&i_array)[Len])
    {
        mpData = &i_array[0];
        mSize = Len;
        return *this;
    }
	
private:
    const T* mpData;
    std::uint32_t mSize;
};


template<typename T, typename U>
inline bool operator ==(const ArrayPtr<T>& i_lhs, const ArrayPtr<U>& i_rhs)
{
    return (i_lhs.Data() == i_rhs.Data()) && (i_lhs.Size() == i_rhs.Size());
}

template<typename T, typename U>
inline bool operator !=(const ArrayPtr<T>& i_lhs, const ArrayPtr<U>& i_rhs)
{
    return !(i_lhs == i_rhs);
}

/*****************************************************************************/

namespace gen
{
	
ArrayPtr<const std::uint8_t> GetFile();
ArrayPtr<const std::uint8_t> GetFileSignature();
std::uint32_t GetFileCrc();

} // end of gen namespace

//=============================================================================
// End of file Embedded.h
//=============================================================================
