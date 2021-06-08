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
#include "locale_matcher_test.h"

#include <climits>
#include <cstring>
#include <gtest/gtest.h>

#include "hilog_wrapper.h"
#include "res_locale.h"
#include "locale_matcher.h"
#include "test_common.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

class LocaleMatcherTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void LocaleMatcherTest::SetUpTestCase(void)
{
    // step 1: input testsuit setup step
    g_logLevel = LOG_DEBUG;
}

void LocaleMatcherTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void LocaleMatcherTest::SetUp()
{
}

void LocaleMatcherTest::TearDown()
{
}

