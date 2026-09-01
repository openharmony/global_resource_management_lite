/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hap_parser.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unzip.h>
#include <zip.h>
#include <zlib.h>

#include "hilog_wrapper.h"
#include "locale_matcher.h"
#include "securec.h"
#include "utils/common.h"
#include "utils/errors.h"
#include "utils/string_utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
const char *HapParser::RES_FILE_NAME = "/resources.index";

struct ResBuffer {
    const char *buffer;
    size_t bufLen;
};

static constexpr size_t UINT16_SIZE = sizeof(uint16_t);
static constexpr size_t NULL_TERMINATOR_SIZE = 1;

static inline bool IsOutOfBounds(uint32_t offset, size_t len, size_t bufLen)
{
    return static_cast<uint64_t>(offset) + static_cast<uint64_t>(len) > static_cast<uint64_t>(bufLen);
}

void UnzCloseFileAndLog(unzFile uf, const char *zipFile)
{
    int err = unzCloseCurrentFile(uf);
    if (err != UNZ_OK) {
        HILOG_ERROR("Error %d with zipfile %s in unzCloseCurrentFile", err, zipFile);
    }
}

static unzFile OpenAndLocateZipFile(const char *zipFile, const char *fileName, std::string &errInfo)
{
    unzFile uf = unzOpen64(zipFile);
    if (uf == nullptr) {
        errInfo = FormatString("Cannot open %s", zipFile);
        return nullptr;
    }
    if (unzLocateFile(uf, fileName, 1)) {
        unzClose(uf);
        errInfo = FormatString("File %s not found in %s", fileName, zipFile);
        return nullptr;
    }
    return uf;
}

int32_t HapParser::ReadFileFromZip(const char *zipFile, const char *fileName, void **buffer,
                                   size_t &bufLen, std::string &errInfo)
{
    unzFile uf = OpenAndLocateZipFile(zipFile, fileName, errInfo);
    if (uf == nullptr) {
        return UNKNOWN_ERROR;
    }
    char filenameInzip[256] = {0};
    unz_file_info fileInfo;
    int err = unzGetCurrentFileInfo(uf, &fileInfo, filenameInzip, sizeof(filenameInzip), nullptr, 0, nullptr, 0);
    if (err != UNZ_OK) {
        unzClose(uf);
        errInfo = FormatString("Error %d with zipfile %s in unzGetCurrentFileInfo.", err, zipFile);
        return UNKNOWN_ERROR;
    }
    if (fileInfo.uncompressed_size == 0 || fileInfo.uncompressed_size > MAX_INDEX_FILE_SIZE) {
        unzClose(uf);
        errInfo = FormatString("invalid file size in zipfile %s", zipFile);
        return UNKNOWN_ERROR;
    }
    *buffer = static_cast<void *>(malloc(fileInfo.uncompressed_size));
    bufLen = fileInfo.uncompressed_size;
    if (*buffer == nullptr) {
        unzClose(uf);
        errInfo = FormatString("Error allocating memory for read buffer");
        return UNKNOWN_ERROR;
    }
    err = unzOpenCurrentFilePassword(uf, nullptr);
    if (err != UNZ_OK) {
        errInfo = FormatString("Error %d with zipfile %s in unzOpenCurrentFilePassword.", err, zipFile);
        free(*buffer);
        *buffer = nullptr;
        unzClose(uf);
        return UNKNOWN_ERROR;
    }
    HILOG_DEBUG("Extracting: %s from %s, file size: %lu", filenameInzip, zipFile, fileInfo.uncompressed_size);
    err = unzReadCurrentFile(uf, *buffer, bufLen);
    if (err < 0) {
        errInfo = FormatString("Error %d with zipfile %s in unzReadCurrentFile", err, zipFile);
        free(*buffer);
        *buffer = nullptr;
        UnzCloseFileAndLog(uf, zipFile);
        unzClose(uf);
        return UNKNOWN_ERROR;
    }
    UnzCloseFileAndLog(uf, zipFile);
    unzClose(uf);
    return OK;
}

std::string GetModuleName(const char *configStr)
{
    std::string config(configStr);
    static const char *key = "\"moduleName\"";
    auto idx = config.find(key);
    if (idx == std::string::npos) {
        return std::string();
    }
    auto start = config.find("\"", idx + strlen(key));
    if (start == std::string::npos) {
        return std::string();
    }
    auto end = config.find("\"", start + 1);
    if (end == std::string::npos) {
        return std::string();
    }

    std::string retStr = std::string(configStr + start + 1, end - start - 1);
    return retStr;
}

int32_t HapParser::ReadIndexFromFile(const char *zipFile, void **buffer,
                                     size_t &bufLen, std::string &errInfo)
{
    void *tmpBuf = nullptr;
    size_t tmpLen;
    std::string tmp;
    int32_t ret = ReadFileFromZip(zipFile, "config.json", &tmpBuf, tmpLen, tmp);
    if (ret != OK) {
        errInfo = "read config.json error";
        HILOG_ERROR("read config.json error");
        return ret;
    }

    // parse config.json
    std::string mName = GetModuleName(static_cast<char *>(tmpBuf));
    if (mName.size() == 0) {
        errInfo = "parse moduleName from config.json error";
        free(tmpBuf);
        return UNKNOWN_ERROR;
    }
    free(tmpBuf);
    std::string indexFilePath = std::string("assets/");
    indexFilePath.append(mName);
    indexFilePath.append(RES_FILE_NAME);

    return ReadFileFromZip(zipFile, indexFilePath.c_str(), buffer, bufLen, errInfo);
}

/**
 *
 * @param buffer
 * @param offset
 * @param id
 * @param includeTemi dose length include '\0'
 * @return OK or ERROR
 */
int32_t ParseString(const ResBuffer &buf, uint32_t &offset, std::string &id, bool includeTemi = true)
{
    if (IsOutOfBounds(offset, UINT16_SIZE, buf.bufLen)) {
        HILOG_ERROR("ParseString length failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    uint16_t strLen;
    errno_t eret = memcpy_s(&strLen, sizeof(strLen), buf.buffer + offset, UINT16_SIZE);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    if (includeTemi && strLen == 0) {
        return SYS_ERROR;
    }
    offset += UINT16_SIZE;
    size_t readLen = includeTemi ? strLen : static_cast<size_t>(strLen) + NULL_TERMINATOR_SIZE;
    if (IsOutOfBounds(offset, readLen, buf.bufLen)) {
        HILOG_ERROR("ParseString value failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    size_t strDataLen = includeTemi ? strLen - NULL_TERMINATOR_SIZE : strLen;
    std::string tmp = std::string(const_cast<char *>(buf.buffer) + offset, strDataLen);
    offset += readLen;
    id = tmp;
    return OK;
}

/**
 *
 * @param buffer
 * @param offset
 * @param values
 * @return
 */
int32_t ParseStringArray(const ResBuffer &buf, uint32_t &offset, std::vector<std::string> &values)
{
    if (IsOutOfBounds(offset, UINT16_SIZE, buf.bufLen)) {
        HILOG_ERROR("ParseStringArray failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    uint16_t arrLen;
    errno_t eret = memcpy_s(&arrLen, sizeof(arrLen), buf.buffer + offset, UINT16_SIZE);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    offset += UINT16_SIZE;
    if (arrLen == 0 || IsOutOfBounds(offset, arrLen, buf.bufLen)) {
        HILOG_ERROR("ParseStringArray failed, arrLen out of bounds");
        return SYS_ERROR;
    }
    uint32_t startOffset = offset;
    while (true) {
        std::string value;
        int32_t ret = ParseString(buf, offset, value, false);
        if (ret != OK) {
            return ret;
        }
        values.push_back(value);

        uint32_t readSize = offset - startOffset;
        if (readSize + NULL_TERMINATOR_SIZE == arrLen) {
            offset += NULL_TERMINATOR_SIZE; // skip trailing '\0'
            break;
        }
        if (readSize + NULL_TERMINATOR_SIZE > arrLen) {
            return SYS_ERROR;
        }
    }

    return OK;
}

int32_t ParseIdItem(const ResBuffer &buf, uint32_t &offset, IdItem *idItem)
{
    if (IsOutOfBounds(offset, IdItem::HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse IdItemHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(idItem, sizeof(IdItem), buf.buffer + offset, IdItem::HEADER_LEN);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    if (static_cast<uint32_t>(idItem->resType_) >= static_cast<uint32_t>(ResType::MAX_RES_TYPE)) {
        HILOG_ERROR("ParseIdItem failed, invalid resType");
        return SYS_ERROR;
    }
    offset += IdItem::HEADER_LEN;

    idItem->JudgeArray();
    if (idItem->isArray_) {
        int32_t ret = ParseStringArray(buf, offset, idItem->values_);
        if (ret != OK) {
            return ret;
        }
    } else {
        std::string value;
        int32_t ret = ParseString(buf, offset, value);
        if (ret != OK) {
            return ret;
        }
        idItem->value_ = std::string(value);
        idItem->valueLen_ = value.size();
    }
    std::string name;
    int32_t ret = ParseString(buf, offset, name);
    if (ret != OK) {
        return ret;
    }
    idItem->name_ = std::string(name);
    return OK;
}

int32_t ParseIdParam(const ResBuffer &buf, uint32_t &offset, IdParam *ip)
{
    if (IsOutOfBounds(offset, ResId::IDPARAM_HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse IdParam failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(ip, sizeof(IdParam), buf.buffer + offset, ResId::IDPARAM_HEADER_LEN);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    offset += ResId::IDPARAM_HEADER_LEN;

    IdItem *idItem = new (std::nothrow) IdItem();
    if (idItem == nullptr) {
        HILOG_ERROR("new IdItem failed when ParseIdParam");
        return SYS_ERROR;
    }
    uint32_t ipOffset = ip->offset_;
    int32_t ret = ParseIdItem(buf, ipOffset, idItem);
    if (ret != OK) {
        delete (idItem);
        return ret;
    }
    ip->idItem_ = idItem;
    return OK;
}

int32_t ParseId(const ResBuffer &buf, uint32_t &offset, ResId *id)
{
    if (IsOutOfBounds(offset, ResId::RESID_HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse ResIdHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(id, sizeof(ResId), buf.buffer + offset, ResId::RESID_HEADER_LEN);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    offset += ResId::RESID_HEADER_LEN;
    if (id->tag_[0] != 'I' || id->tag_[1] != 'D'
        || id->tag_[2] != 'S' || id->tag_[3] != 'S') {
        return -1;
    }
    if (id->count_ > MAX_RES_ID_COUNT) {
        HILOG_ERROR("Parse ResId failed, id count out of range");
        return SYS_ERROR;
    }
    for (uint32_t i = 0; i < id->count_; ++i) {
        IdParam *ip = new (std::nothrow) IdParam();
        if (ip == nullptr) {
            HILOG_ERROR("new IdParam failed when ParseId");
            return SYS_ERROR;
        }
        int32_t ret = ParseIdParam(buf, offset, ip);
        if (ret != OK) {
            delete (ip);
            return ret;
        }
        id->idParams_.push_back(ip);
    }

    return OK;
}

bool IsLocaleMatch(const ResConfigImpl *defaultConfig, const std::vector<KeyParam *> &keyParams)
{
    if (defaultConfig != nullptr) {
        ResConfigImpl *config = HapParser::CreateResConfigFromKeyParams(keyParams);
        if (config == nullptr) {
            return SYS_ERROR;
        }
        if (!LocaleMatcher::Match(defaultConfig->GetResLocale(), config->GetResLocale())) {
            HILOG_DEBUG("mismatch, do not parse %s", HapParser::ToFolderPath(keyParams).c_str());
            delete (config);
            return false;
        }
        delete (config);
    }
    return true;
}

int32_t ParseKeyParam(const ResBuffer &buf, uint32_t &offset, KeyParam *kp)
{
    if (IsOutOfBounds(offset, ResKey::KEYPARAM_HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse KeyParam failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(kp, sizeof(KeyParam), buf.buffer + offset, ResKey::KEYPARAM_HEADER_LEN);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    offset += ResKey::KEYPARAM_HEADER_LEN;
    kp->InitStr();
    return OK;
}

int32_t ParseKey(const ResBuffer &buf, uint32_t &offset, ResKey *key,
                 bool &match, const ResConfigImpl *defaultConfig)
{
    if (IsOutOfBounds(offset, ResKey::RESKEY_HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse ResKeyHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(key, sizeof(ResKey), buf.buffer + offset, ResKey::RESKEY_HEADER_LEN);
    if (eret != EOK) {
        return SYS_ERROR;
    }
    offset += ResKey::RESKEY_HEADER_LEN;
    if (key->tag_[0] != 'K' || key->tag_[1] != 'E'
        || key->tag_[2] != 'Y' || key->tag_[3] != 'S') {
        return -1;
    }
    if (key->keyParamsCount_ > MAX_KEY_PARAMS_COUNT) {
        HILOG_ERROR("Parse ResKey failed, keyParamsCount out of range");
        return SYS_ERROR;
    }
    for (uint32_t i = 0; i < key->keyParamsCount_; ++i) {
        KeyParam *kp = new (std::nothrow) KeyParam();
        if (kp == nullptr) {
            HILOG_ERROR("new KeyParam failed when ParseKey");
            return SYS_ERROR;
        }
        int32_t ret = ParseKeyParam(buf, offset, kp);
        if (ret != OK) {
            delete (kp);
            return ret;
        }
        key->keyParams_.push_back(kp);
    }
    match = IsLocaleMatch(defaultConfig, key->keyParams_);
    if (!match) {
        return OK;
    }
    uint32_t idOffset = key->offset_;
    ResId *id = new (std::nothrow) ResId();
    if (id == nullptr) {
        HILOG_ERROR("new ResId failed when ParseKey");
        return SYS_ERROR;
    }
    int32_t ret = ParseId(buf, idOffset, id);
    if (ret != OK) {
        delete (id);
        return ret;
    }
    key->resId_ = id;
    return OK;
}


int32_t HapParser::ParseResHex(const char *buffer, const size_t bufLen, ResDesc &resDesc,
                               const ResConfigImpl *defaultConfig)
{
    ResBuffer buf = {buffer, bufLen};
    ResHeader *resHeader = new (std::nothrow) ResHeader();
    if (resHeader == nullptr) {
        HILOG_ERROR("new ResHeader failed when ParseResHex");
        return SYS_ERROR;
    }
    uint32_t offset = 0;
    if (IsOutOfBounds(offset, RES_HEADER_LEN, buf.bufLen)) {
        HILOG_ERROR("Parse ResHeader failed, the offset will be out of bounds");
        delete (resHeader);
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(resHeader, sizeof(ResHeader), buf.buffer + offset, RES_HEADER_LEN);
    if (eret != EOK) {
        delete (resHeader);
        return SYS_ERROR;
    }
    offset += RES_HEADER_LEN;
    if (resHeader->keyCount_ == 0 || resHeader->keyCount_ > MAX_RES_KEY_COUNT || resHeader->length_ == 0) {
        delete (resHeader);
        return UNKNOWN_ERROR;
    }

    resDesc.resHeader_ = resHeader;
    for (uint32_t i = 0; i < resHeader->keyCount_; i++) {
        ResKey *key = new (std::nothrow) ResKey();
        if (key == nullptr) {
            HILOG_ERROR("new ResKey failed when ParseResHex");
            return SYS_ERROR;
        }
        bool match = true;
        int32_t ret = ParseKey(buf, offset, key, match, defaultConfig);
        if (ret != OK) {
            delete (key);
            return ret;
        }
        if (match) {
            resDesc.keys_.push_back(key);
        } else {
            delete (key);
        }
    }
    return OK;
}

ResConfigImpl *HapParser::CreateResConfigFromKeyParams(const std::vector<KeyParam *> &keyParams)
{
    ResConfigImpl *resConfig = new (std::nothrow) ResConfigImpl;
    if (resConfig == nullptr) {
        HILOG_ERROR("new ResConfigImpl failed when CreateResConfigFromKeyParams");
        return nullptr;
    }
    size_t len = keyParams.size();
    // default path
    if (len == 0) {
        return resConfig;
    }
    size_t i = 0;
    const char *language = nullptr;
    const char *script = nullptr;
    const char *region = nullptr;
    ScreenDensity screenDensity = SCREEN_DENSITY_NOT_SET;
    Direction direction = DIRECTION_NOT_SET;
    DeviceType deviceType = DEVICE_NOT_SET;

    for (i = 0; i < len; ++i) {
        const KeyParam *kp = keyParams.at(i);
        if (kp->type_ == LANGUAGES) {
            language = kp->GetStr().c_str();
        } else if (kp->type_ == REGION) {
            region = kp->GetStr().c_str();
        } else if (kp->type_ == SCRIPT) {
            script = kp->GetStr().c_str();
        } else if (kp->type_ == SCREEN_DENSITY) {
            screenDensity = GetScreenDensity(kp->value_);
        } else if (kp->type_ == DEVICETYPE) {
            deviceType = GetDeviceType(kp->value_);
        } else if (kp->type_ == DIRECTION) {
            if (kp->value_ == 0) {
                direction = DIRECTION_VERTICAL;
            } else {
                direction = DIRECTION_HORIZONTAL;
            }
        }
    }
    resConfig->SetDeviceType(deviceType);
    resConfig->SetDirection(direction);
    resConfig->SetScreenDensity(screenDensity);
    RState r = resConfig->SetLocaleInfo(language, script, region);
    if (r != SUCCESS) {
        HILOG_ERROR("error set locale,lang %s,script %s,region %s", language, script,
            region);
    }

    return resConfig;
}

DeviceType HapParser::GetDeviceType(uint32_t value)
{
    DeviceType deviceType = DEVICE_NOT_SET;
    if (value == DEVICE_CAR) {
        deviceType = DEVICE_CAR;
    } else if (value == DEVICE_PAD) {
        deviceType = DEVICE_PAD;
    } else if (value == DEVICE_PHONE) {
        deviceType = DEVICE_PHONE;
    } else if (value == DEVICE_TABLET) {
        deviceType = DEVICE_TABLET;
    } else if (value == DEVICE_TV) {
        deviceType = DEVICE_TV;
    } else if (value == DEVICE_WEARABLE) {
        deviceType = DEVICE_WEARABLE;
    }
    return deviceType;
}

ScreenDensity HapParser::GetScreenDensity(uint32_t value)
{
    ScreenDensity screenDensity = SCREEN_DENSITY_NOT_SET;
    if (value == SCREEN_DENSITY_SDPI) {
        screenDensity = SCREEN_DENSITY_SDPI;
    } else if (value == SCREEN_DENSITY_MDPI) {
        screenDensity = SCREEN_DENSITY_MDPI;
    } else if (value == SCREEN_DENSITY_LDPI) {
        screenDensity = SCREEN_DENSITY_LDPI;
    } else if (value == SCREEN_DENSITY_XLDPI) {
        screenDensity = SCREEN_DENSITY_XLDPI;
    } else if (value == SCREEN_DENSITY_XXLDPI) {
        screenDensity = SCREEN_DENSITY_XXLDPI;
    } else if (value == SCREEN_DENSITY_XXXLDPI) {
        screenDensity = SCREEN_DENSITY_XXXLDPI;
    }
    return screenDensity;
}

void PathAppend(std::string &path, const std::string &append, const std::string &connector)
{
    if (append.size() > 0) {
        if (path.size() > 0) {
            path.append(connector);
        }
        path.append(append);
    }
}

std::string HapParser::ToFolderPath(const std::vector<KeyParam *> &keyParams)
{
    if (keyParams.size() == 0) {
        return std::string("default");
    }
    // language_script_region-direction-deviceType-screenDensity
    std::string language;
    std::string script;
    std::string region;
    std::string direction;
    std::string deviceType;
    std::string screenDensity;
    for (size_t i = 0; i < keyParams.size(); ++i) {
        KeyParam *keyParam = keyParams[i];
        switch (keyParam->type_) {
            case KeyType::LANGUAGES:
                language = keyParam->GetStr();
                break;
            case KeyType::SCRIPT:
                script = keyParam->GetStr();
                break;
            case KeyType::REGION:
                region = keyParam->GetStr();
                break;
            case KeyType::DIRECTION:
                direction = keyParam->GetStr();
                break;
            case KeyType::DEVICETYPE:
                deviceType = keyParam->GetStr();
                break;
            case KeyType::SCREEN_DENSITY:
                screenDensity = keyParam->GetStr();
                break;
            default:
                break;
        }
    }
    std::string path;
    if (language.size() > 0) {
        path.append(language);
    }
    std::string c1("_");
    std::string c2("-");
    PathAppend(path, script, c1);
    PathAppend(path, region, c1);
    PathAppend(path, direction, c2);
    PathAppend(path, deviceType, c2);
    PathAppend(path, screenDensity, c2);

    return path;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS