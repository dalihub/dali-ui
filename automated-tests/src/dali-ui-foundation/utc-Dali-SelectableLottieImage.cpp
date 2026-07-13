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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/public-api/actors/actor.h>
#include <dali.h>

#include <type_traits>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

// The "url without both ranges" case is a COMPILE-TIME guarantee: the default constructor
// is deleted, so a SelectableLottieImage can never be created without a url and both ranges.
static_assert(!std::is_default_constructible<SelectableLottieImage>::value,
              "SelectableLottieImage must not be default-constructible");

void utc_dali_selectable_lottie_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectable_lottie_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

// The enforced url + both ranges construction path round-trips through the accessors.
// (The negative "url without ranges" case is a COMPILE-TIME guarantee: the default
// constructor is deleted, so it cannot be exercised as a runtime test case.)
int UtcDaliSelectableLottieImageConstructRoundTripP(void)
{
  UiTestApplication application;

  SelectableLottieImage image("checkbox.json",
                              SelectableLottieImage::FrameRange(0, 30),
                              SelectableLottieImage::FrameRange(30, 48));

  DALI_TEST_EQUALS(image.GetUrl(), std::string("checkbox.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectRange().startFrame, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectRange().endFrame, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectRange().startFrame, 30, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectRange().endFrame, 48, TEST_LOCATION);

  END_TEST;
}

// The optional inner-fill key path round-trips through the 4-arg constructor and defaults to
// empty when omitted (the renderer then falls back to its generic default path).
int UtcDaliSelectableLottieImageInnerFillKeyPathP(void)
{
  UiTestApplication application;

  // 3-arg construction: the key path defaults to empty.
  SelectableLottieImage defaulted("checkbox.json",
                                  SelectableLottieImage::FrameRange(0, 19),
                                  SelectableLottieImage::FrameRange(20, 38));
  DALI_TEST_EQUALS(defaulted.GetInnerFillKeyPath(), std::string(""), TEST_LOCATION);

  // 4-arg construction: the key path round-trips verbatim (the trailing space is preserved).
  SelectableLottieImage keyed("checkbox.json",
                              SelectableLottieImage::FrameRange(0, 19),
                              SelectableLottieImage::FrameRange(20, 38),
                              "check_box .inner_fill.color");
  DALI_TEST_EQUALS(keyed.GetInnerFillKeyPath(), std::string("check_box .inner_fill.color"), TEST_LOCATION);

  END_TEST;
}

// FrameRange default-constructs to (0, 0) and the two-arg constructor sets both members.
int UtcDaliSelectableLottieImageFrameRangeP(void)
{
  UiTestApplication application;

  SelectableLottieImage::FrameRange defaultRange;
  DALI_TEST_EQUALS(defaultRange.startFrame, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultRange.endFrame, 0, TEST_LOCATION);

  SelectableLottieImage::FrameRange range(7, 19);
  DALI_TEST_EQUALS(range.startFrame, 7, TEST_LOCATION);
  DALI_TEST_EQUALS(range.endFrame, 19, TEST_LOCATION);

  END_TEST;
}

// Copy and move both reproduce the same url + ranges.
int UtcDaliSelectableLottieImageCopyMoveP(void)
{
  UiTestApplication application;

  SelectableLottieImage image("radio.json",
                              SelectableLottieImage::FrameRange(1, 12),
                              SelectableLottieImage::FrameRange(12, 24));

  SelectableLottieImage copy(image);
  DALI_TEST_EQUALS(copy.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSelectRange().startFrame, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSelectRange().endFrame, 12, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDeselectRange().startFrame, 12, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDeselectRange().endFrame, 24, TEST_LOCATION);

  SelectableLottieImage assigned = image;
  DALI_TEST_EQUALS(assigned.GetUrl(), std::string("radio.json"), TEST_LOCATION);

  SelectableLottieImage moved(std::move(copy));
  DALI_TEST_EQUALS(moved.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetSelectRange().endFrame, 12, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDeselectRange().endFrame, 24, TEST_LOCATION);

  END_TEST;
}
