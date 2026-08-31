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

#include <dali-ui-foundation/public-api/types/selectable-lottie-color-binding.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <type_traits>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using Binding     = SelectableLottieColorBinding;
using ColorPolicy = Binding::ColorPolicy;
using FrameRange  = SelectableLottieImage::FrameRange;

constexpr char MOVED_FROM_ASSERTION[] = "Cannot use a moved-from SelectableLottieColorBinding object";
} //namespace

static_assert(!std::is_default_constructible<Binding>::value,
              "A selectable Lottie color binding requires a non-empty key path");

void utc_dali_selectable_lottie_color_binding_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectable_lottie_color_binding_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSelectableLottieColorBindingRoundTripP(void)
{
  UiTestApplication application;
  Binding           binding("radio_outline.outline_stroke.color",
                            LottieAnimation::VectorProperty::STROKE_COLOR,
                            ColorPolicy::SELECTED_IN_FRAME_RANGE,
                            FrameRange(7, 26));

  DALI_TEST_EQUALS(binding.GetKeyPath(), std::string("radio_outline.outline_stroke.color"), TEST_LOCATION);
  DALI_TEST_CHECK(binding.GetProperty() == LottieAnimation::VectorProperty::STROKE_COLOR);
  DALI_TEST_CHECK(binding.GetColorPolicy() == ColorPolicy::SELECTED_IN_FRAME_RANGE);
  DALI_TEST_EQUALS(binding.GetSelectedColorRange().startFrame, 7, TEST_LOCATION);
  DALI_TEST_EQUALS(binding.GetSelectedColorRange().endFrame, 26, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSelectableLottieColorBindingReverseRangeNormalizesP(void)
{
  UiTestApplication application;
  Binding           binding("outline",
                            LottieAnimation::VectorProperty::STROKE_COLOR,
                            ColorPolicy::SELECTED_IN_FRAME_RANGE,
                            FrameRange(26, 7));

  DALI_TEST_EQUALS(binding.GetSelectedColorRange().startFrame, 7, TEST_LOCATION);
  DALI_TEST_EQUALS(binding.GetSelectedColorRange().endFrame, 26, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSelectableLottieColorBindingCopyMoveP(void)
{
  UiTestApplication application;
  Binding           source("inner", LottieAnimation::VectorProperty::FILL_COLOR, ColorPolicy::ALWAYS_SELECTED);
  Binding           copied(source);

  DALI_TEST_EQUALS(copied.GetKeyPath(), std::string("inner"), TEST_LOCATION);
  DALI_TEST_CHECK(copied.GetColorPolicy() == ColorPolicy::ALWAYS_SELECTED);

  Binding assigned("temporary", LottieAnimation::VectorProperty::FILL_COLOR, ColorPolicy::ALWAYS_DESELECTED);
  assigned = source;
  DALI_TEST_EQUALS(assigned.GetKeyPath(), std::string("inner"), TEST_LOCATION);

  Binding moved(std::move(copied));
  DALI_TEST_EQUALS(moved.GetKeyPath(), std::string("inner"), TEST_LOCATION);
  DALI_TEST_ASSERTION(copied.GetKeyPath(), MOVED_FROM_ASSERTION);

  // Copy assignment can restore a moved-from destination.
  copied = source;
  DALI_TEST_EQUALS(copied.GetKeyPath(), std::string("inner"), TEST_LOCATION);

  Binding moveAssigned("other", LottieAnimation::VectorProperty::STROKE_COLOR, ColorPolicy::BY_SELECTION_STATE);
  moveAssigned = std::move(assigned);
  DALI_TEST_EQUALS(moveAssigned.GetKeyPath(), std::string("inner"), TEST_LOCATION);
  DALI_TEST_ASSERTION(assigned.GetProperty(), MOVED_FROM_ASSERTION);
  END_TEST;
}

int UtcDaliSelectableLottieColorBindingEmptyKeyPathN(void)
{
  UiTestApplication application;
  DALI_TEST_ASSERTION(
    Binding("", LottieAnimation::VectorProperty::FILL_COLOR, ColorPolicy::BY_SELECTION_STATE),
    "key path must not be empty");
  END_TEST;
}

int UtcDaliSelectableLottieColorBindingNonColorPropertyN(void)
{
  UiTestApplication application;
  DALI_TEST_ASSERTION(
    Binding("inner", LottieAnimation::VectorProperty::FILL_OPACITY, ColorPolicy::BY_SELECTION_STATE),
    "supports only fill and stroke color properties");
  END_TEST;
}
