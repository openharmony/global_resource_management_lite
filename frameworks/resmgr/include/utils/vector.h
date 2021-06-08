/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <sys/types.h>

#include <utils/log.h>
#include <utils/type_helpers.h>
#include <utils/vector_impl.h>

/*
 * Used to blacklist some functions from CFI.
 *
 */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(no_sanitize)
#define UTILS_VECTOR_NO_CFI __attribute__((no_sanitize("cfi")))
#else
#define UTILS_VECTOR_NO_CFI
#endif

// ---------------------------------------------------------------------------

namespace OHOS {
template<typename TYPE>
class SortedVector;

/*!
 * The main templated vector class ensuring type safety
 * while making use of VectorImpl.
 * This is the class users want to use.
 *
 * DO NOT USE: please use std::vector
 */

template<class TYPE>
class Vector : private VectorImpl {
public:
    typedef TYPE value_type;

    /*!
     * Constructors and destructors
     */

    Vector();

    Vector(const Vector<TYPE> &rhs);

    explicit Vector(const SortedVector<TYPE> &rhs);

    virtual                 ~Vector();

    /*! copy operator */

    const Vector<TYPE> &operator=(const Vector<TYPE> &rhs) const;

    Vector<TYPE> &operator=(const Vector<TYPE> &rhs);

    const Vector<TYPE> &operator=(const SortedVector<TYPE> &rhs) const;

    Vector<TYPE> &operator=(const SortedVector<TYPE> &rhs);

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
     * set the size of the vector. items are appended with the default
     * constructor, or removed from the end as needed.
     */
    inline ssize_t Resize(size_t size)
    { return VectorImpl::Resize(size); }

    /*!
     * C-style array access
     */

    //! read-only C-style access
    inline const TYPE *Array() const;

    //! read-write C-style access
    TYPE *EditArray();

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

    //! copy-on write support, grants write access to an item
    TYPE &EditItemAt(size_t index);

    //! grants right access to the top of the stack (last element)
    TYPE &EditTop();

    /*!
     * append/insert another vector
     */

    //! insert another vector at a given index
    ssize_t InsertVectorAt(const Vector<TYPE> &vector, size_t index);

    //! append another vector at the end of this one
    ssize_t AppendVector(const Vector<TYPE> &vector);

    //! insert an array at a given index
    ssize_t InsertArrayAt(const TYPE *array, size_t index, size_t length);

    //! append an array at the end of this vector
    ssize_t AppendArray(const TYPE *array, size_t length);

    /*!
     * add/insert/replace items
     */
    //! insert one or several items initialized with their default constructor
    inline ssize_t InsertAt(size_t index, size_t numItems = 1);

    //! insert one or several items initialized from a prototype item
    ssize_t InsertAt(const TYPE &prototype_item, size_t index, size_t numItems = 1);

    //! pop the top of the stack (removes the last element). No-op if the stack's empty
    inline void Pop();

    //! pushes an item initialized with its default constructor
    inline void Push();

    //! pushes an item on the top of the stack
    void Push(const TYPE &item);

    //! same as push() but returns the index the item was added at (or an error)
    inline ssize_t Add();

    //! same as push() but returns the index the item was added at (or an error)
    ssize_t Add(const TYPE &item);

    //! replace an item with a new one initialized with its default constructor
    inline ssize_t ReplaceAt(size_t index);

    //! replace an item with a new one
    ssize_t ReplaceAt(const TYPE &item, size_t index);

    /*!
     * remove items
     */

    //! remove several items
    inline ssize_t RemoveItemsAt(size_t index, size_t count = 1);

    //! remove one item
    inline ssize_t RemoveAt(size_t index)
    { return RemoveItemsAt(index); }

    /*!
     * sort (stable) the array
     */

    typedef int (*compar_t)(const TYPE *lhs, const TYPE *rhs);

    typedef int (*compar_r_t)(const TYPE *lhs, const TYPE *rhs, void *state);

    inline int32_t Sort(compar_t cmp);

    inline int32_t Sort(compar_r_t cmp, void *state);

    /*
     * these inlines add some level of compatibility with STL. eventually
     * we should probably turn things around.
     */
    typedef TYPE* iterator;
    typedef const TYPE* const_iterator;

    inline iterator begin()
    { return EditArray(); }

    inline iterator end()
    { return EditArray() + Size(); }

    inline const_iterator begin() const
    { return Array(); }

    inline const_iterator end() const
    { return Array() + Size(); }

    inline void reserve(size_t n)
    { SetCapacity(n); }

    inline bool empty() const
    { return IsEmpty(); }

    inline void push_back(const TYPE &item)
    { InsertAt(item, Size(), 1); }

    inline void push_front(const TYPE &item)
    { InsertAt(item, 0, 1); }

    inline iterator erase(iterator pos)
    {
        ssize_t index = RemoveItemsAt(static_cast<size_t>(pos - Array()));
        return begin() + index;
    }

protected:
    virtual void DoConstruct(void *storage, size_t num) const;

    virtual void DoDestroy(void *storage, size_t num) const;

    virtual void DoCopy(void *dest, const void *from, size_t num) const;

    virtual void DoSplat(void *dest, const void *item, size_t num) const;

    virtual void DoMoveForward(void *dest, const void *from, size_t num) const;

    virtual void DoMoveBackward(void *dest, const void *from, size_t num) const;
};

// ---------------------------------------------------------------------------
// No user serviceable parts from here...
// ---------------------------------------------------------------------------

template<class TYPE>
inline Vector<TYPE>::Vector()
    : VectorImpl(sizeof(TYPE),
                 (((traits<TYPE>::has_trivial_ctor) ? HAS_TRIVIAL_CTOR : 0)
                  | ((traits<TYPE>::has_trivial_dtor) ? HAS_TRIVIAL_DTOR : 0)
                  | ((traits<TYPE>::has_trivial_copy) ? HAS_TRIVIAL_COPY : 0))
)
{
}

template<class TYPE>
inline Vector<TYPE>::Vector(const Vector<TYPE> &rhs)
    : VectorImpl(rhs)
{
}

template<class TYPE>
inline Vector<TYPE>::Vector(const SortedVector<TYPE> &rhs)
    : VectorImpl(static_cast<const VectorImpl &>(rhs))
{
}

template<class TYPE>
inline Vector<TYPE>::~Vector()
{
    FinishVector();
}

template<class TYPE>
inline Vector<TYPE> &Vector<TYPE>::operator=(const Vector<TYPE> &rhs)
{
    VectorImpl::operator=(rhs);
    return *this;
}

template<class TYPE>
inline Vector<TYPE> &Vector<TYPE>::operator=(const SortedVector<TYPE> &rhs)
{
    VectorImpl::operator=(static_cast<const VectorImpl &>(rhs));
    return *this;
}

template<class TYPE>
inline const Vector<TYPE> &Vector<TYPE>::operator=(const SortedVector<TYPE> &rhs) const
{
    VectorImpl::operator=(rhs);
    return *this;
}

template<class TYPE>
inline const TYPE *Vector<TYPE>::Array() const
{
    return static_cast<const TYPE *>(ArrayImpl());
}

template<class TYPE>
inline TYPE *Vector<TYPE>::EditArray()
{
    return static_cast<TYPE *>(EditArrayImpl());
}


template<class TYPE>
inline const TYPE &Vector<TYPE>::operator[](size_t index) const
{
    LOG_FATAL_IF(index >= Size(),
                 "%s: index=%u out of range (%u)", __PRETTY_FUNCTION__,
                 int(index), int (size()));
    return *(Array() + index);
}

template<class TYPE>
inline const TYPE &Vector<TYPE>::ItemAt(size_t index) const
{
    return operator[](index);
}

template<class TYPE>
inline const TYPE &Vector<TYPE>::Top() const
{
    return *(Array() + Size() - 1);
}

template<class TYPE>
inline TYPE &Vector<TYPE>::EditItemAt(size_t index)
{
    return *(static_cast<TYPE *>(EditItemLocation(index)));
}

template<class TYPE>
inline TYPE &Vector<TYPE>::EditTop()
{
    return *(static_cast<TYPE *>(EditItemLocation(Size() - 1)));
}

template<class TYPE>
inline ssize_t Vector<TYPE>::InsertVectorAt(const Vector<TYPE> &vector, size_t index)
{
    return VectorImpl::InsertVectorAt(reinterpret_cast<const VectorImpl &>(vector), index);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::AppendVector(const Vector<TYPE> &vector)
{
    return VectorImpl::AppendVector(reinterpret_cast<const VectorImpl &>(vector));
}

template<class TYPE>
inline ssize_t Vector<TYPE>::InsertArrayAt(const TYPE *array, size_t index, size_t length)
{
    return VectorImpl::InsertArrayAt(array, index, length);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::AppendArray(const TYPE *array, size_t length)
{
    return VectorImpl::AppendArray(array, length);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::InsertAt(const TYPE &item, size_t index, size_t numItems)
{
    return VectorImpl::InsertAt(&item, index, numItems);
}

template<class TYPE>
inline void Vector<TYPE>::Push(const TYPE &item)
{
    return VectorImpl::Push(&item);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::Add(const TYPE &item)
{
    return VectorImpl::Add(&item);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::ReplaceAt(const TYPE &item, size_t index)
{
    return VectorImpl::ReplaceAt(&item, index);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::InsertAt(size_t index, size_t numItems)
{
    return VectorImpl::InsertAt(index, numItems);
}

template<class TYPE>
inline void Vector<TYPE>::Pop()
{
    VectorImpl::Pop();
}

template<class TYPE>
inline void Vector<TYPE>::Push()
{
    VectorImpl::Push();
}

template<class TYPE>
inline ssize_t Vector<TYPE>::Add()
{
    return VectorImpl::Add();
}

template<class TYPE>
inline ssize_t Vector<TYPE>::ReplaceAt(size_t index)
{
    return VectorImpl::ReplaceAt(index);
}

template<class TYPE>
inline ssize_t Vector<TYPE>::RemoveItemsAt(size_t index, size_t count)
{
    return VectorImpl::RemoveItemsAt(index, count);
}

template<class TYPE>
inline int32_t Vector<TYPE>::Sort(Vector<TYPE>::compar_t cmp)
{
    return VectorImpl::Sort(reinterpret_cast<VectorImpl::compar_t>(cmp));
}

template<class TYPE>
inline int32_t Vector<TYPE>::Sort(Vector<TYPE>::compar_r_t cmp, void *state)
{
    return VectorImpl::Sort(reinterpret_cast<VectorImpl::compar_r_t>(cmp), state);
}

// ---------------------------------------------------------------------------

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::DoConstruct(void *storage, size_t num) const
{
    construct_type(reinterpret_cast<TYPE *>(storage), num);
}

template<class TYPE>
void Vector<TYPE>::DoDestroy(void *storage, size_t num) const
{
    destroy_type(reinterpret_cast<TYPE *>(storage), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::DoCopy(void *dest, const void *from, size_t num) const
{
    copy_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::DoSplat(void *dest, const void *item, size_t num) const
{
    splat_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(item), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::DoMoveForward(void *dest, const void *from, size_t num) const
{
    move_forward_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::DoMoveBackward(void *dest, const void *from, size_t num) const
{
    move_backward_type(reinterpret_cast<TYPE *>(dest), reinterpret_cast<const TYPE *>(from), num);
}
}  // namespace OHOS
