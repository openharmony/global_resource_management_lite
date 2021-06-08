/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>

#include <utils/log.h>
#include <utils/type_helpers.h>
#include <utils/vector.h>
#include <utils/vector_impl.h>

// ---------------------------------------------------------------------------

namespace OHOS {
template<class TYPE>
class SortedVector : private SortedVectorImpl {
    friend class Vector<TYPE>;

public:
    typedef TYPE value_type;

    /*!
     * Constructors and destructors
     */

    SortedVector();

    SortedVector(const SortedVector<TYPE> &rhs);

    virtual                 ~SortedVector();

    /*! copy operator */
    const SortedVector<TYPE> &operator=(const SortedVector<TYPE> &rhs) const;

    SortedVector<TYPE> &operator=(const SortedVector<TYPE> &rhs);

    /*
     * empty the vector
     */

    inline void Clear()
    { VectorImpl::Clear(); }

    /*!
     * vector stats
     */

    //! returns number of items in the vector
    inline size_t Size() const
    { return VectorImpl::Size(); }

    //! returns whether or not the vector is empty
    inline bool IsEmpty() const
    { return VectorImpl::IsEmpty(); }

    //! returns how many items can be stored without reallocating the backing store
    inline size_t Capacity() const
    { return VectorImpl::Capacity(); }

    //! sets the capacity. capacity can never be reduced less than size()
    inline ssize_t SetCapacity(size_t size)
    { return VectorImpl::SetCapacity(size); }

    /*!
     * C-style array access
     */

    //! read-only C-style access
    inline const TYPE *Array() const;

    //! read-write C-style access. BE VERY CAREFUL when modifying the array
    //! you must keep it sorted! You usually don't use this function.
    TYPE *EditArray();

    //! finds the index of an item
    ssize_t IndexOf(const TYPE &item) const;

    //! finds where this item should be inserted
    size_t OrderOf(const TYPE &item) const;

    /*!
     * accessors
     */

    //! read-only access to an item at a given index
    inline const TYPE &operator[](size_t index) const;

    //! alternate name for operator []
    inline const TYPE &ItemAt(size_t index) const;

    //! stack-usage of the vector. returns the top of the stack (last element)
    const TYPE &Top() const;

    /*!
     * modifying the array
     */

    //! add an item in the right place (and replace the one that is there)
    ssize_t Add(const TYPE &item);

    //! editItemAt() MUST NOT change the order of this item
    TYPE &EditItemAt(size_t index)
    {
        return *(static_cast<TYPE *>(VectorImpl::EditItemLocation(index)));
    }

    //! merges a vector into this one
    ssize_t Merge(const Vector<TYPE> &vector);

    ssize_t Merge(const SortedVector<TYPE> &vector);

    //! removes an item
    ssize_t Remove(const TYPE &);

    //! remove several items
    inline ssize_t RemoveItemsAt(size_t index, size_t count = 1);

    //! remove one item
    inline ssize_t RemoveAt(size_t index)
    { return RemoveItemsAt(index); }

    /*
     * these inlines add some level of compatibility with STL.
     */
    typedef TYPE* iterator;
    typedef const TYPE* const_iterator;

    inline iterator Begin()
    { return EditArray(); }

    inline iterator End()
    { return EditArray() + Size(); }

    inline const_iterator Begin() const
    { return Array(); }

    inline const_iterator End() const
    { return Array() + Size(); }

    inline void Reserve(size_t n)
    { SetCapacity(n); }

    inline bool Empty() const
    { return IsEmpty(); }

    inline iterator Erase(iterator pos)
    {
        ssize_t index = RemoveItemsAt(pos - Array());
        return Begin() + index;
    }

protected:
    virtual void DoConstruct(void *storage, size_t num) const;

    virtual void DoDestroy(void *storage, size_t num) const;

    virtual void DoCopy(void *dest, const void *from, size_t num) const;

    virtual void DoSplat(void *dest, const void *item, size_t num) const;

    virtual void DoMoveForward(void *dest, const void *from, size_t num) const;

    virtual void DoMoveBackward(void *dest, const void *from, size_t num) const;

    virtual int DoCompare(const void *lhs, const void *rhs) const;
};

// ---------------------------------------------------------------------------
// No user serviceable parts from here...
// ---------------------------------------------------------------------------

template<class TYPE>
inline SortedVector<TYPE>::SortedVector()
    : SortedVectorImpl(sizeof(TYPE),
                       (((traits<TYPE>::has_trivial_ctor) ? HAS_TRIVIAL_CTOR : 0)
                        | ((traits<TYPE>::has_trivial_dtor) ? HAS_TRIVIAL_DTOR : 0)
                        | ((traits<TYPE>::has_trivial_copy) ? HAS_TRIVIAL_COPY : 0))
)
{
}

template<class TYPE>
inline SortedVector<TYPE>::SortedVector(const SortedVector<TYPE> &rhs)
    : SortedVectorImpl(rhs)
{
}

template<class TYPE>
inline SortedVector<TYPE>::~SortedVector()
{
    FinishVector();
}

template<class TYPE>
inline SortedVector<TYPE> &SortedVector<TYPE>::operator=(const SortedVector<TYPE> &rhs)
{
    SortedVectorImpl::operator=(rhs);
    return *this;
}

template<class TYPE>
inline const SortedVector<TYPE> &SortedVector<TYPE>::operator=(const SortedVector<TYPE> &rhs) const
{
    SortedVectorImpl::operator=(rhs);
    return *this;
}

template<class TYPE>
inline const TYPE *SortedVector<TYPE>::Array() const
{
    return static_cast<const TYPE *>(ArrayImpl());
}

template<class TYPE>
inline TYPE *SortedVector<TYPE>::EditArray()
{
    return static_cast<TYPE *>(EditArrayImpl());
}


template<class TYPE>
inline const TYPE &SortedVector<TYPE>::operator[](size_t index) const
{
    LOG_FATAL_IF(index >= Size(),
                 "%s: index=%u out of range (%u)", __PRETTY_FUNCTION__,
                 int(index), int (size()));
    return *(Array() + index);
}

template<class TYPE>
inline const TYPE &SortedVector<TYPE>::ItemAt(size_t index) const
{
    return operator[](index);
}

template<class TYPE>
inline const TYPE &SortedVector<TYPE>::Top() const
{
    return *(Array() + Size() - 1);
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::Add(const TYPE &item)
{
    return SortedVectorImpl::Add(&item);
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::IndexOf(const TYPE &item) const
{
    return SortedVectorImpl::IndexOf(&item);
}

template<class TYPE>
inline size_t SortedVector<TYPE>::OrderOf(const TYPE &item) const
{
    return SortedVectorImpl::OrderOf(&item);
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::Merge(const Vector<TYPE> &vector)
{
    return SortedVectorImpl::Merge(reinterpret_cast<const VectorImpl &>(vector));
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::Merge(const SortedVector<TYPE> &vector)
{
    return SortedVectorImpl::Merge(reinterpret_cast<const SortedVectorImpl &>(vector));
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::Remove(const TYPE &item)
{
    return SortedVectorImpl::Remove(&item);
}

template<class TYPE>
inline ssize_t SortedVector<TYPE>::RemoveItemsAt(size_t index, size_t count)
{
    return VectorImpl::RemoveItemsAt(index, count);
}

// ---------------------------------------------------------------------------

template<class TYPE>
UTILS_VECTOR_NO_CFI void SortedVector<TYPE>::DoConstruct(void *storage, size_t num) const
{
    construct_type(reinterpret_cast<TYPE *>(storage), num);
}

template<class TYPE>
void SortedVector<TYPE>::DoDestroy(void *storage, size_t num) const
{
    destroy_type(reinterpret_cast<TYPE *>(storage), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void SortedVector<TYPE>::DoCopy(void *dest, const void *from, size_t num) const
{
    copy_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void SortedVector<TYPE>::DoSplat(void *dest, const void *item, size_t num) const
{
    splat_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(item), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void SortedVector<TYPE>::DoMoveForward(void *dest, const void *from, size_t num) const
{
    move_forward_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void SortedVector<TYPE>::DoMoveBackward(void *dest, const void *from, size_t num) const
{
    move_backward_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}

template<class TYPE>
int SortedVector<TYPE>::DoCompare(const void *lhs, const void *rhs) const
{
    return compare_type(*reinterpret_cast<const TYPE *>(lhs), *reinterpret_cast<const TYPE *>(rhs));
}
}  // namespace OHOS
