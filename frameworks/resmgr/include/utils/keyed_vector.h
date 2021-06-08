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

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>

#include <utils/log.h>
#include <utils/errors.h>
#include <utils/sorted_vector.h>
#include <utils/type_helpers.h>

// ---------------------------------------------------------------------------

namespace OHOS {
template<typename KEY, typename VALUE>
class KeyedVector {
public:
    typedef KEY key_type;
    typedef VALUE value_type;

    inline KeyedVector();

    /*
     * empty the vector
     */

    inline void Clear()
    { mVector_.Clear(); }

    /*! 
     * vector stats
     */

    //! returns number of items in the vector
    inline size_t Size() const
    { return mVector_.Size(); }

    //! returns whether or not the vector is empty
    inline bool IsEmpty() const
    { return mVector_.IsEmpty(); }

    //! returns how many items can be stored without reallocating the backing store
    inline size_t Capacity() const
    { return mVector_.Capacity(); }

    //! sets the capacity. capacity can never be reduced less than size()
    inline ssize_t SetCapacity(size_t size)
    { return mVector_.SetCapacity(size); }

    // returns true if the arguments is known to be identical to this vector
    inline bool IsIdenticalTo(const KeyedVector &rhs) const;

    /*! 
     * accessors
     */
    const VALUE &ValueFor(const KEY &key) const;

    const VALUE &ValueAt(size_t index) const;

    const KEY &KeyAt(size_t index) const;

    ssize_t IndexOfKey(const KEY &key) const;

    const VALUE &operator[](size_t index) const;

    /*!
     * modifying the array
     */

    VALUE &EditValueFor(const KEY &key);

    VALUE &EditValueAt(size_t index);

    /*!
     * add/insert/replace items
     */

    ssize_t Add(const KEY &key, const VALUE &item);

    ssize_t ReplaceValueFor(const KEY &key, const VALUE &item);

    ssize_t ReplaceValueAt(size_t index, const VALUE &item);

    /*!
     * remove items
     */

    ssize_t RemoveItem(const KEY &key);

    ssize_t RemoveItemsAt(size_t index, size_t count = 1);

private:
    SortedVector<key_value_pair_t<KEY, VALUE>> mVector_;
};

// ---------------------------------------------------------------------------

/**
 * Variation of KeyedVector that holds a default value to return when
 * valueFor() is called with a key that doesn't exist.
 */
template<typename KEY, typename VALUE>
class DefaultKeyedVector : public KeyedVector<KEY, VALUE> {
public:
    inline DefaultKeyedVector(const VALUE &defValue = VALUE());

    const VALUE &ValueFor(const KEY &key) const;

private:
    VALUE mDefault_;
};

// ---------------------------------------------------------------------------

template<typename KEY, typename VALUE>
inline KeyedVector<KEY, VALUE>::KeyedVector()
{
}

template<typename KEY, typename VALUE>
inline bool KeyedVector<KEY, VALUE>::IsIdenticalTo(const KeyedVector<KEY, VALUE> &rhs) const
{
    return mVector_.Array() == rhs.mVector_.Array();
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::IndexOfKey(const KEY &key) const
{
    return mVector_.IndexOf(key_value_pair_t<KEY, VALUE>(key));
}

template<typename KEY, typename VALUE>
inline const VALUE &KeyedVector<KEY, VALUE>::ValueFor(const KEY &key) const
{
    ssize_t i = this->IndexOfKey(key);
    LOG_ALWAYS_FATAL_IF(i < 0, "%s: key not found", __PRETTY_FUNCTION__);
    return mVector_.ItemAt(i).value;
}

template<typename KEY, typename VALUE>
inline const VALUE &KeyedVector<KEY, VALUE>::ValueAt(size_t index) const
{
    return mVector_.ItemAt(index).value;
}

template<typename KEY, typename VALUE>
inline const VALUE &KeyedVector<KEY, VALUE>::operator[](size_t index) const
{
    return ValueAt(index);
}

template<typename KEY, typename VALUE>
inline const KEY &KeyedVector<KEY, VALUE>::KeyAt(size_t index) const
{
    return mVector_.ItemAt(index).key;
}

template<typename KEY, typename VALUE>
inline VALUE &KeyedVector<KEY, VALUE>::EditValueFor(const KEY &key)
{
    ssize_t i = this->IndexOfKey(key);
    LOG_ALWAYS_FATAL_IF(i < 0, "%s: key not found", __PRETTY_FUNCTION__);
    return mVector_.EditItemAt(static_cast<size_t>(i)).value;
}

template<typename KEY, typename VALUE>
inline VALUE &KeyedVector<KEY, VALUE>::EditValueAt(size_t index)
{
    return mVector_.EditItemAt(index).value;
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::Add(const KEY &key, const VALUE &value)
{
    return mVector_.Add(key_value_pair_t<KEY, VALUE>(key, value));
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::ReplaceValueFor(const KEY &key, const VALUE &value)
{
    key_value_pair_t<KEY, VALUE> pair(key, value);
    mVector_.Remove(pair);
    return mVector_.Add(pair);
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::ReplaceValueAt(size_t index, const VALUE &item)
{
    if (index < Size()) {
        mVector_.EditItemAt(index).value = item;
        return static_cast<ssize_t>(index);
    }
    return BAD_INDEX;
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::RemoveItem(const KEY &key)
{
    return mVector_.Remove(key_value_pair_t<KEY, VALUE>(key));
}

template<typename KEY, typename VALUE>
inline ssize_t KeyedVector<KEY, VALUE>::RemoveItemsAt(size_t index, size_t count)
{
    return mVector_.RemoveItemsAt(index, count);
}

// ---------------------------------------------------------------------------

template<typename KEY, typename VALUE>
inline DefaultKeyedVector<KEY, VALUE>::DefaultKeyedVector(const VALUE &defValue)
    : mDefault_(defValue)
{
}

template<typename KEY, typename VALUE>
inline const VALUE &DefaultKeyedVector<KEY, VALUE>::ValueFor(const KEY &key) const
{
    ssize_t i = this->IndexOfKey(key);
    return (i >= 0) ? KeyedVector<KEY, VALUE>::ValueAt(i) : mDefault_;
}
}  // namespace OHOS
