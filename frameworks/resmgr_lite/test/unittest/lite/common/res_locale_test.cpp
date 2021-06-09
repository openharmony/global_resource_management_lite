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
#include "res_locale_test.h"

#include <chrono>
#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "hilog_wrapper.h"
#include "res_locale.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class ResLocaleTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void ResLocaleTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
}

void ResLocaleTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void ResLocaleTest::SetUp()
{
}

void ResLocaleTest::TearDown()
{
}

/*
 * @tc.name:ResLocaleFindAndSortTest001
 * @tc.desc: Test AddResource
 * @tc.type: FUNC
 */
HWTEST_F(ResLocaleTest, ResLocaleFindAndSortTest001, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CN");
    request.push_back("en-US");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-US";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en-US");
    EXPECT_TRUE(outValue.at(1) == "en");
    EXPECT_TRUE(outValue.at(2) == "");
    EXPECT_TRUE(outValue.at(3) == "en-CN");
    EXPECT_TRUE(outValue.at(4) == "en-GB");
}

HWTEST_F(ResLocaleTest, ResLocaleFindAndSortTest002, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("zh-CN");
    request.push_back("zh-TW");
    request.push_back("zh");
    request.push_back("zh-HK");
    request.push_back("");
    std::string current = "zh-CN";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "zh-CN");
    EXPECT_TRUE(outValue.at(1) == "zh");
    EXPECT_TRUE(outValue.at(2) == "");
}

HWTEST_F(ResLocaleTest, ResLocaleFindAndSortTest003, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-CN";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en");
    EXPECT_TRUE(outValue.at(1) == "en-CA");
    EXPECT_TRUE(outValue.at(2) == "");
    EXPECT_TRUE(outValue.at(3) == "en-GB");
}

HWTEST_F(ResLocaleTest, ResLocaleFindAndSortTest004, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-Qaag";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en");
    EXPECT_TRUE(outValue.at(1) == "en-GB");
    EXPECT_TRUE(outValue.at(2) == "en-CA");
    EXPECT_TRUE(outValue.at(3) == "");
}

HWTEST_F(ResLocaleTest, ResLocaleFindAndSortTest005, TestSize.Level1)
{
    std::vector<std::string> request;
    std::vector<std::string> outValue;
    request.push_back("en");
    request.push_back("en-001");
    request.push_back("en-CA");
    request.push_back("en-GB");
    request.push_back("");
    std::string current = "en-AI";
    FindAndSort(current, request, outValue);
    EXPECT_TRUE(outValue.at(0) == "en-001");
    EXPECT_TRUE(outValue.at(1) == "en");
    EXPECT_TRUE(outValue.at(2) == "en-GB");
    EXPECT_TRUE(outValue.at(3) == "en-CA");
    EXPECT_TRUE(outValue.at(4) == "");
}

HWTEST_F(ResLocaleTest, ResLocaleGetSysDefaultTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* localeInfo = BuildFromString("zh-CN", '-', state);
    if (localeInfo == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    UpdateSysDefault(*localeInfo, false);
    const LocaleInfo* currentresLocale = GetSysDefault();
    if (currentresLocale == nullptr) {
        EXPECT_TRUE(false);
        delete localeInfo;
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", currentresLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("CN", currentresLocale->GetRegion()) == 0);
    delete localeInfo;
    localeInfo = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleUpdateSysDefaultTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    LocaleInfo* resLocale = BuildFromString("zh-CN", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    UpdateSysDefault(*resLocale, false);
    const LocaleInfo* currentresLocale = GetSysDefault();
    if (currentresLocale == nullptr) {
        EXPECT_TRUE(false);
        delete resLocale;
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", currentresLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("CN", currentresLocale->GetRegion()) == 0);
    resLocale = BuildFromString("en-US", '-', state);
    UpdateSysDefault(*resLocale, false);
    currentresLocale = GetSysDefault();
    if (currentresLocale == nullptr) {
        EXPECT_TRUE(false);
        delete resLocale;
        return;
    }
    EXPECT_TRUE(std::strcmp("en", currentresLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("US", currentresLocale->GetRegion()) == 0);
    resLocale = BuildFromString("en-Qaag-US", '-', state);
    UpdateSysDefault(*resLocale, false);
    currentresLocale = GetSysDefault();
    EXPECT_TRUE(std::strcmp("en", currentresLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("US", currentresLocale->GetRegion()) == 0);
    EXPECT_TRUE(std::strcmp("Qaag", currentresLocale->GetScript()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleGetLanguageTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-CN", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleGetRegionTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-CN", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("CN", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleGetScriptTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-Hant-CN", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(std::strcmp("Hant", resLocale->GetScript()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts("zh", "Hant", "CN", state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Hant", resLocale->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("CN", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest002, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts("zh1", "Hant", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest003, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts("zh", "Hants", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_SCRIPT_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest004, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts("zh", "Hant", "C", state);
    EXPECT_TRUE(state == INVALID_BCP47_REGION_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest005, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts(nullptr, "Hants", "CN", state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromParsTest006, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromParts("zh", nullptr, nullptr, state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(resLocale->GetScript() == nullptr);
    EXPECT_TRUE(resLocale->GetRegion() == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest001, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-Hant-CN", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Hant", resLocale->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("CN", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest002, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh1-Hant-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest003, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("-Hant-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_LANGUAGE_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest004, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(resLocale->GetScript() == nullptr);
    EXPECT_TRUE(resLocale->GetRegion() == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest005, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("en_Latn_US", '_', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("en", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Latn", resLocale->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("US", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest006, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("en_Latn_US", '&', state);
    EXPECT_TRUE(state == NOT_SUPPORT_SEP);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest007, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("en_Latn_US", '_', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("en", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(std::strcmp("Latn", resLocale->GetScript()) == 0);
    EXPECT_TRUE(std::strcmp("US", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest008, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-Hants-CN", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_SCRIPT_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest009, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-Hant-C", '-', state);
    EXPECT_TRUE(state == INVALID_BCP47_REGION_SUBTAG);
    EXPECT_TRUE(resLocale == nullptr);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocaleBuildFromStringTest0010, TestSize.Level1)
{
    RState state = SUCCESS;
    ResLocale* resLocale = ResLocale::BuildFromString("zh-CN-xxxx", '-', state);
    if (resLocale == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_TRUE(state == SUCCESS);
    EXPECT_TRUE(std::strcmp("zh", resLocale->GetLanguage()) == 0);
    EXPECT_TRUE(resLocale->GetScript() == nullptr);
    EXPECT_TRUE(std::strcmp("CN", resLocale->GetRegion()) == 0);
    delete resLocale;
    resLocale = nullptr;
}

HWTEST_F(ResLocaleTest, ResLocalePerformanceFuncTest001, TestSize.Level1)
{
    unsigned long long total = 0;
    double average = 0;
    std::vector<std::string> outValue;
    for (int k = 0; k < 1000; ++k) {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<std::string> request;
        std::vector<std::string> outValue;
        request.push_back("en");
        request.push_back("en-CN");
        request.push_back("en-US");
        request.push_back("en-GB");
        request.push_back("");
        std::string current = "en-US";
        FindAndSort(current, request, outValue);
        auto t2 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }
    average = total / 1000.0;
    HILOG_INFO("avg cost: %f us", average);
    EXPECT_LT(average, 500);
};