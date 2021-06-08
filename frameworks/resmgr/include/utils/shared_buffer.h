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

#ifndef OHOS_SHARED_BUFFER_H
#define OHOS_SHARED_BUFFER_H

#include <atomic>
#include <stdint.h>
#include <sys/types.h>
#include "common.h"

// ---------------------------------------------------------------------------

namespace OHOS {
class SharedBuffer {
public:

    /* flags to use with release() */
    enum {
        eKeepStorage = 0x00000001
    };

    /*! allocate a buffer of size 'size' and acquire() it.
     *  call release() to free it.
     */
    static SharedBuffer *Alloc(size_t size);

    /*! free the memory associated with the SharedBuffer.
     * Fails if there are any users associated with this SharedBuffer.
     * In other words, the buffer must have been release by all its
     * users.
     */
    static void Dealloc(const SharedBuffer *released);

    //! access the data for read
    inline const void *Data() const;

    //! access the data for read/write
    inline void *Data();

    //! get size of the buffer
    inline size_t Size() const;

    //! get back a SharedBuffer object from its data
    static inline SharedBuffer *BufferFromData(void *data);

    //! get back a SharedBuffer object from its data
    static inline const SharedBuffer *BufferFromData(const void *data);

    //! get the size of a SharedBuffer object from its data
    static inline size_t SizeFromData(const void *data);

    //! edit the buffer (get a writtable, or non-const, version of it)
    SharedBuffer *Edit() const;

    //! edit the buffer, resizing if needed
    SharedBuffer *EditResize(size_t size) const;

    //! like edit() but fails if a copy is required
    SharedBuffer *AttemptEdit() const;

    //! resize and edit the buffer, loose it's content.
    SharedBuffer *Reset(size_t size) const;

    //! acquire/release a reference on this buffer
    void Acquire() const;

    /*! release a reference on this buffer, with the option of not
     * freeing the memory associated with it if it was the last reference
     * returns the previous reference count
     */
    int32_t Release(uint32_t flags = 0) const;

    //! returns wether or not we're the only owner
    inline bool OnlyOwner() const;

private:
    inline SharedBuffer()
    {}

    inline ~SharedBuffer()
    {}

    SharedBuffer(const SharedBuffer &);

    SharedBuffer &operator=(const SharedBuffer &);

    // Must be sized to preserve correct alignment.
    mutable std::atomic<int32_t> mRefs_;
    size_t mSize_;
    uint32_t mReserved_[2];
};

static_assert(sizeof(SharedBuffer) % EIGHT == 0
              && (sizeof(size_t) > FOUR || sizeof(SharedBuffer) == SIXTEEN),
              "SharedBuffer has unexpected size");

// ---------------------------------------------------------------------------

const void *SharedBuffer::Data() const
{
    return this + 1;
}

void *SharedBuffer::Data()
{
    return this + 1;
}

size_t SharedBuffer::Size() const
{
    return mSize_;
}

SharedBuffer *SharedBuffer::BufferFromData(void *data)
{
    return data ? (static_cast<SharedBuffer *>(data) - 1) : nullptr;
}

const SharedBuffer *SharedBuffer::BufferFromData(const void *data)
{
    return data ? (static_cast<const SharedBuffer *>(data) - 1) : nullptr;
}

size_t SharedBuffer::SizeFromData(const void *data)
{
    return data ? BufferFromData(data)->mSize_ : 0;
}

bool SharedBuffer::OnlyOwner() const
{
    return (mRefs_.load(std::memory_order_acquire) == 1);
}
}  // namespace OHOS

#endif // OHOS_VECTOR_H
