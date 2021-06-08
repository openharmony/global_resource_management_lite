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
#include <utils/errors.h>

// ---------------------------------------------------------------------------
// No user serviceable parts in here...
// ---------------------------------------------------------------------------

namespace OHOS {
/*!
 * Implementation of the guts of the vector<> class
 * this ensures backward binary compatibility and
 * reduces code size.
 * For performance reasons, we expose mStorage and mCount
 * so these fields are set in stone.
 *
 */

class VectorImpl {
public:
    enum { // flags passed to the ctor
        HAS_TRIVIAL_CTOR = 0x00000001,
        HAS_TRIVIAL_DTOR = 0x00000002,
        HAS_TRIVIAL_COPY = 0x00000004,
    };

    VectorImpl(size_t itemSize, uint32_t flags);

    VectorImpl(const VectorImpl &rhs);

    virtual                 ~VectorImpl();

    /*! must be called from subclasses destructor */
    void FinishVector();

    VectorImpl &operator=(const VectorImpl &rhs);

    /*! C-style array access */
    inline const void *ArrayImpl() const
    { return mStorage_; }

    void *EditArrayImpl();

    /*! vector stats */
    inline size_t Size() const
    { return mCount_; }

    inline bool IsEmpty() const
    { return mCount_ == 0; }

    size_t Capacity() const;

    ssize_t SetCapacity(size_t size);

    ssize_t Resize(size_t size);

    /*! append/insert another vector or array */
    ssize_t InsertVectorAt(const VectorImpl &vector, size_t index);

    ssize_t AppendVector(const VectorImpl &vector);

    ssize_t InsertArrayAt(const void *array, size_t index, size_t length);

    ssize_t AppendArray(const void *array, size_t length);

    /*! add/insert/replace items */
    ssize_t InsertAt(size_t where, size_t numItems = 1);

    ssize_t InsertAt(const void *item, size_t where, size_t numItems = 1);

    void Pop();

    void Push();

    void Push(const void *item);

    ssize_t Add();

    ssize_t Add(const void *item);

    ssize_t ReplaceAt(size_t index);

    ssize_t ReplaceAt(const void *item, size_t index);

    /*! remove items */
    ssize_t RemoveItemsAt(size_t index, size_t count = 1);

    void Clear();

    const void *ItemLocation(size_t index) const;

    void *EditItemLocation(size_t index);

    typedef int (*compar_t)(const void *lhs, const void *rhs);

    typedef int (*compar_r_t)(const void *lhs, const void *rhs, void *state);

    int32_t Sort(compar_t cmp);

    int32_t Sort(compar_r_t cmp, void *state);

protected:
    size_t ItemSize() const;

    void ReleaseStorage();

    virtual void DoConstruct(void *storage, size_t num) const = 0;

    virtual void DoDestroy(void *storage, size_t num) const = 0;

    virtual void DoCopy(void *dest, const void *from, size_t num) const = 0;

    virtual void DoSplat(void *dest, const void *item, size_t num) const = 0;

    virtual void DoMoveForward(void *dest, const void *from, size_t num) const = 0;

    virtual void DoMoveBackward(void *dest, const void *from, size_t num) const = 0;

private:
    void *GrowP(size_t where, size_t amount);

    void ShrinkP(size_t where, size_t amount);

    inline void DoConstructP(void *storage, size_t num) const;

    inline void DoDestroyP(void *storage, size_t num) const;

    inline void DoCopyP(void *dest, const void *from, size_t num) const;

    inline void DoSplatP(void *dest, const void *item, size_t num) const;

    inline void DoMoveForwardP(void *dest, const void *from, size_t num) const;

    inline void DoMoveBackwardP(void *dest, const void *from, size_t num) const;

    // These 2 fields are exposed in the inlines below,
    // so they're set in stone.
    void *mStorage_;   // base address of the vector
    size_t mCount_;     // number of items

    const uint32_t mFlags_;
    const size_t mItemSize_;
};


class SortedVectorImpl : public VectorImpl {
public:
    SortedVectorImpl(size_t itemSize, uint32_t flags);

    explicit SortedVectorImpl(const VectorImpl &rhs);

    virtual                 ~SortedVectorImpl();

    SortedVectorImpl &operator=(const SortedVectorImpl &rhs);

    //! finds the index of an item
    ssize_t IndexOf(const void *item) const;

    //! finds where this item should be inserted
    size_t OrderOf(const void *item) const;

    //! add an item in the right place (or replaces it if there is one)
    ssize_t Add(const void *item);

    //! merges a vector into this one
    ssize_t Merge(const VectorImpl &vector);

    ssize_t Merge(const SortedVectorImpl &vector);

    //! removes an item
    ssize_t Remove(const void *item);

protected:
    virtual int DoCompare(const void *lhs, const void *rhs) const = 0;

private:
    ssize_t IndexOrderOf(const void *item, size_t *order = nullptr) const;

    // these are made private, because they can't be used on a SortedVector
    // (they don't have an implementation either)
    ssize_t Add();

    void Pop();

    void Push();

    void Push(const void *item);

    ssize_t InsertVectorAt(const VectorImpl &vector, size_t index);

    ssize_t AppendVector(const VectorImpl &vector);

    ssize_t InsertArrayAt(const void *array, size_t index, size_t length);

    ssize_t AppendArray(const void *array, size_t length);

    ssize_t InsertAt(size_t where, size_t numItems = 1);

    ssize_t InsertAt(const void *item, size_t where, size_t numItems = 1);

    ssize_t ReplaceAt(size_t index);

    ssize_t ReplaceAt(const void *item, size_t index);
};
}  // namespace OHOS
