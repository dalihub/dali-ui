/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define private public
#include <dali-ui-foundation/internal/feedback/feedback-style.h>
#undef private

#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

void utc_dali_feedback_style_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_feedback_style_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFeedbackStyleThemeAndPatternP(void)
{
  UiTestApplication application(UiConfig::New());
  FeedbackStyle style;
  const std::string theme = R"JSON({
    "style": {
      "Actor": {
        "signals": {
          "first": {
            "type": "onScene",
            "hapticFeedbackPattern": "FEEDBACK_PATTERN_TAP",
            "soundFeedbackPattern": "FEEDBACK_PATTERN_MESSAGE"
          },
          "replacement": {
            "type": "onScene",
            "hapticFeedbackFile": "/tmp/haptic-feedback"
          },
          "file": {
            "type": "offScene",
            "hapticFeedbackFile": "/tmp/combined-feedback"
          },
          "ignored": {
            "type": "touched",
            "hapticFeedbackPattern": 12
          }
        }
      }
    }
  })JSON";

  DALI_TEST_CHECK(style.LoadTheme(theme));
  DALI_TEST_CHECK(!style.mStyleInfoLut.empty());
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_TAP"), FEEDBACK_PATTERN_TAP, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_SLIDER_SWEEP"), FEEDBACK_PATTERN_SLIDER_SWEEP, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("unknown"), FEEDBACK_PATTERN_NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetFeedbackPattern("FEEDBACK_PATTERN_KEY5"), FEEDBACK_PATTERN_KEY5, TEST_LOCATION);

  style.PlayFeedback("Actor", "onScene");
  style.PlayFeedback("Actor", "offScene");
  style.PlayFeedback("Actor", "missing");
  style.PlayFeedback("MissingType", "onScene");
  END_TEST;
}

int UtcDaliFeedbackStyleObjectAndReloadP(void)
{
  UiTestApplication application(UiConfig::New());
  FeedbackStyle style;
  DALI_TEST_CHECK(style.LoadTheme(R"JSON({"style":{"Actor":{"signals":{"empty":{"type":"onScene"}}}}})JSON"));

  style.ObjectCreated(BaseHandle());
  Actor actor = Actor::New();
  style.ObjectCreated(actor);
  style.StyleChanged("/tmp/no-feedback-theme.json", StyleChange::DEFAULT_FONT_CHANGE);
  style.StyleChanged("/tmp/no-feedback-theme.json", StyleChange::THEME_CHANGE);

  DALI_TEST_CHECK(style.LoadTheme("not-json"));
  DALI_TEST_CHECK(!style.LoadTheme(R"JSON({"style":{"Actor":{"signals":{"broken":{}}}}})JSON"));
  style.LoadFromString("still-not-json");
  END_TEST;
}
