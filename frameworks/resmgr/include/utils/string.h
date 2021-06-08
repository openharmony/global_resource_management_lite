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

#ifndef OHOS_STRING8_H
#define OHOS_STRING8_H


#include <utils/errors.h>
#include <utils/type_helpers.h>

#include <string.h> // for strcmp
#include <stdarg.h>
// ---------------------------------------------------------------------------

namespace OHOS {
// DO NOT USE: please use std::string

//! This is a string holding UTF-8 characters. Does not allow the value more
// than 0x10FFFF, which is not valid unicode codepoint.
class String {
public:
    String();

    String(const String &o);

    explicit String(const char *o);

    explicit String(const char *o, size_t numChars);

    ~String();

    static inline const String Empty();

    static String Format(const char *fmt, ...) __attribute__((format (printf, 1, 2)));

    static String FormatV(const char *fmt, va_list args);

    inline const char *CStr() const;

public:

    inline size_t Size() const;

    inline size_t Bytes() const;

    inline bool IsEmpty() const;

    size_t Length() const;

    void Clear();

    void SetTo(const String &other);

    int32_t SetTo(const char *other);

    int32_t SetTo(const char *other, size_t numChars);

    int32_t Append(const String &other);

    int32_t Append(const char *other);

    int32_t Append(const char *other, size_t numChars);

    int32_t AppendFormat(const char *fmt, ...)
    __attribute__((format (printf, 2, 3)));

    int32_t AppendFormatV(const char *fmt, va_list args);

    inline String &operator=(const String &other);

    inline String &operator=(const char *other);

    inline String &operator+=(const String &other);

    inline String operator+(const String &other) const;

    inline String &operator+=(const char *other);

    inline String operator+(const char *other) const;

    inline int Compare(const String &other) const;

    inline bool operator<(const String &other) const;

    inline bool operator<=(const String &other) const;

    inline bool operator==(const String &other) const;

    inline bool operator!=(const String &other) const;

    inline bool operator>=(const String &other) const;

    inline bool operator>(const String &other) const;

    inline bool operator<(const char *other) const;

    inline bool operator<=(const char *other) const;

    inline bool operator==(const char *other) const;

    inline bool operator!=(const char *other) const;

    inline bool operator>=(const char *other) const;

    inline bool operator>(const char *other) const;

    inline                      operator const char *() const;

    char *LockBuffer(size_t size);

    void UnlockBuffer();

    int32_t UnlockBuffer(size_t size);

    // return the index of the first byte of other in this at or after
    // start, or -1 if not found
    ssize_t Find(const char *other, size_t start = 0) const;

    // return true if this string contains the specified substring
    inline bool Contains(const char *other) const;

    // removes all occurrence of the specified substring
    // returns true if any were found and removed
    bool RemoveAll(const char *other);

    void ToLower();

    void ToLower(size_t start, size_t numChars);

    void ToUpper();

    void ToUpper(size_t start, size_t numChars);

private:
    int32_t RealAppend(const char *other, size_t numChars);

    const char *mString_;
};

// String can be trivially moved using memcpy() because moving does not
// require any change to the underlying SharedBuffer contents or reference count.
OHOS_TRIVIAL_MOVE_TRAIT(String)


inline const String String::Empty()
{
    return String();
}

inline const char *String::CStr() const
{
    return mString_;
}

inline size_t String::Size() const
{
    return Length();
}

inline bool String::IsEmpty() const
{
    return Length() == 0;
}

inline size_t String::Bytes() const
{
    return Length();
}

inline bool String::Contains(const char *other) const
{
    return Find(other) >= 0;
}

inline String &String::operator=(const String &other)
{
    SetTo(other);
    return *this;
}

inline String &String::operator=(const char *other)
{
    SetTo(other);
    return *this;
}

inline String &String::operator+=(const String &other)
{
    Append(other);
    return *this;
}

inline String String::operator+(const String &other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline String &String::operator+=(const char *other)
{
    Append(other);
    return *this;
}

inline String String::operator+(const char *other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline int String::Compare(const String &other) const
{
    return strcmp(mString_, other.mString_);
}

inline bool String::operator<(const String &other) const
{
    return strcmp(mString_, other.mString_) < 0;
}

inline bool String::operator<=(const String &other) const
{
    return strcmp(mString_, other.mString_) <= 0;
}

inline bool String::operator==(const String &other) const
{
    return strcmp(mString_, other.mString_) == 0;
}

inline bool String::operator!=(const String &other) const
{
    return strcmp(mString_, other.mString_) != 0;
}

inline bool String::operator>=(const String &other) const
{
    return strcmp(mString_, other.mString_) >= 0;
}

inline bool String::operator>(const String &other) const
{
    return strcmp(mString_, other.mString_) > 0;
}

inline bool String::operator<(const char *other) const
{
    return strcmp(mString_, other) < 0;
}

inline bool String::operator<=(const char *other) const
{
    return strcmp(mString_, other) <= 0;
}

inline bool String::operator==(const char *other) const
{
    return strcmp(mString_, other) == 0;
}

inline bool String::operator!=(const char *other) const
{
    return strcmp(mString_, other) != 0;
}

inline bool String::operator>=(const char *other) const
{
    return strcmp(mString_, other) >= 0;
}

inline bool String::operator>(const char *other) const
{
    return strcmp(mString_, other) > 0;
}

inline String::operator const char *() const
{
    return mString_;
}
}  // namespace OHOS
#endif // OHOS_STRING8_H
