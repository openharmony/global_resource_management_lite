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

#include "string_utils_test.h"

#include <climits>
#include <gtest/gtest.h>
#include <locale_info.h>
#include <plural_format.h>
#include <types.h>

#include "resource_manager.h"
#include "test_common.h"
#include "utils/date_utils.h"
#include "utils/string_utils.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;
using namespace OHOS::I18N;

class StringUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void StringUtilsTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void StringUtilsTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void StringUtilsTest::SetUp()
{
    // step 3: input testcase setup step
}

void StringUtilsTest::TearDown()
{
    // step 4: input testcase teardown step
}

std::string ConvertToPluralStr(int idxRet)
{
    switch (idxRet) {
        case OHOS::I18N::PluralRuleType::ZERO:
            return "zero";
        case OHOS::I18N::PluralRuleType::ONE:
            return "one";
        case OHOS::I18N::PluralRuleType::TWO:
            return "two";
        case OHOS::I18N::PluralRuleType::FEW:
            return "few";
        case OHOS::I18N::PluralRuleType::MANY:
            return "many";
        case OHOS::I18N::PluralRuleType::OTHER:
        default:
            return "other";
    }
}
/*
 * @tc.name: StringUtilsFuncTest003
 * @tc.desc: Test icu function, none file case.
 * @tc.type: FUNC
 */
HWTEST_F(StringUtilsTest, StringUtilsFuncTest003, TestSize.Level1)
{
    auto rm = CreateResourceManager(); // just for icu init data
    OHOS::I18N::LocaleInfo locale("am", "", "");
    I18nStatus status = I18nStatus::ISUCCESS;
    PluralFormat formatter(locale, status);

    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 55, 500, 1000};

    int size = sizeof(numbers) / sizeof(numbers[0]);
    for (int i = 0; i < size; ++i) {
        int us = formatter.GetPluralRuleIndex(numbers[i], status);
        std::string converted = ConvertToPluralStr(us);
        HILOG_DEBUG("%d con: %s", numbers[i], converted.c_str());
    }
    delete (rm);
}