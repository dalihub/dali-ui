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
static_assert(sizeof(SelectableLottieImage) == sizeof(void*),
              "SelectableLottieImage must contain only its implementation pointer");
static_assert(sizeof(Int32Pair) == 2 * sizeof(int32_t),
              "Int32Pair must retain its packed 64-bit size");
static_assert(std::is_trivially_copyable<Int32Pair>::value,
              "Int32Pair must remain a lightweight value type");
static_assert(std::is_standard_layout<Int32Pair>::value,
              "Int32Pair must remain a standard-layout value type");
static_assert(std::is_nothrow_move_constructible<SelectableLottieImage>::value &&
                std::is_nothrow_move_assignable<SelectableLottieImage>::value,
              "Moving an image must transfer ownership without throwing");

namespace
{
constexpr char MOVED_FROM_ASSERTION[] = "Cannot use a moved-from SelectableLottieImage object";
} // namespace

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

  Int32Pair selectRange(0, 30);
  SelectableLottieImage image("checkbox.json",
                              selectRange,
                              Int32Pair(30, 48));

  // Mutating the input value must not change the owned description.
  selectRange.SetStart(7);

  DALI_TEST_EQUALS(image.GetUrl(), std::string("checkbox.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectionPlaybackRange().GetStart(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectionPlaybackRange().GetEnd(), 30, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectionPlaybackRange().GetStart(), 30, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectionPlaybackRange().GetEnd(), 48, TEST_LOCATION);

  // Returned ranges are independent values as well.
  Int32Pair returnedRange = image.GetSelectionPlaybackRange();
  returnedRange.SetEnd(12);
  DALI_TEST_EQUALS(returnedRange.GetEnd(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectionPlaybackRange().GetEnd(), 30, TEST_LOCATION);

  END_TEST;
}

// The optional inner-fill key path round-trips through the 4-arg constructor and defaults to
// empty when omitted (the renderer then falls back to its generic default path).
int UtcDaliSelectableLottieImageInnerFillKeyPathP(void)
{
  UiTestApplication application;

  // 3-arg construction: the key path defaults to empty.
  SelectableLottieImage defaulted("checkbox.json",
                                  Int32Pair(0, 19),
                                  Int32Pair(20, 38));
  DALI_TEST_EQUALS(defaulted.GetInnerFillKeyPath(), std::string(""), TEST_LOCATION);

  // 4-arg construction: the key path round-trips verbatim (the trailing space is preserved).
  SelectableLottieImage keyed("checkbox.json",
                              Int32Pair(0, 19),
                              Int32Pair(20, 38),
                              "check_box .inner_fill.color");
  DALI_TEST_EQUALS(keyed.GetInnerFillKeyPath(), std::string("check_box .inner_fill.color"), TEST_LOCATION);

  END_TEST;
}

// Int32Pair preserves the supplied endpoints for consumer-specific normalization.
int UtcDaliSelectableLottieImageInt32PairP(void)
{
  UiTestApplication application;

  Int32Pair defaultRange;
  DALI_TEST_EQUALS(defaultRange.GetStart(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultRange.GetEnd(), 0, TEST_LOCATION);

  Int32Pair rawRange(19, -7);
  DALI_TEST_EQUALS(rawRange.GetStart(), 19, TEST_LOCATION);
  DALI_TEST_EQUALS(rawRange.GetEnd(), -7, TEST_LOCATION);

  Int32Pair range(7, 19);
  DALI_TEST_EQUALS(range.GetStart(), 7, TEST_LOCATION);
  DALI_TEST_EQUALS(range.GetEnd(), 19, TEST_LOCATION);

  END_TEST;
}

// Copy and move both reproduce the same url + ranges.
int UtcDaliSelectableLottieImageCopyMoveP(void)
{
  UiTestApplication application;

  SelectableLottieImage image("radio.json",
                              Int32Pair(1, 12),
                              Int32Pair(12, 24));

  SelectableLottieImage copy(image);
  DALI_TEST_EQUALS(copy.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSelectionPlaybackRange().GetStart(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSelectionPlaybackRange().GetEnd(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDeselectionPlaybackRange().GetStart(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetDeselectionPlaybackRange().GetEnd(), 24, TEST_LOCATION);

  SelectableLottieImage assigned("other.json", {}, {});
  assigned = image;
  DALI_TEST_EQUALS(assigned.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(assigned.GetSelectionPlaybackRange().GetEnd(), 12, TEST_LOCATION);

  // Replacing the source must not change independently owned copies.
  image = SelectableLottieImage("replacement.json", {}, {}, "replacement.fill");
  DALI_TEST_EQUALS(copy.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(assigned.GetUrl(), std::string("radio.json"), TEST_LOCATION);

  SelectableLottieImage moved(std::move(copy));
  DALI_TEST_EQUALS(moved.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetSelectionPlaybackRange().GetEnd(), 12, TEST_LOCATION);
  DALI_TEST_EQUALS(moved.GetDeselectionPlaybackRange().GetEnd(), 24, TEST_LOCATION);
  DALI_TEST_ASSERTION(copy.GetUrl(), MOVED_FROM_ASSERTION);
  DALI_TEST_ASSERTION(copy.GetSelectionPlaybackRange(), MOVED_FROM_ASSERTION);
  DALI_TEST_ASSERTION(copy.GetDeselectionPlaybackRange(), MOVED_FROM_ASSERTION);
  DALI_TEST_ASSERTION(copy.GetInnerFillKeyPath(), MOVED_FROM_ASSERTION);

  // Copy assignment restores a moved-from destination.
  copy = moved;
  DALI_TEST_EQUALS(copy.GetUrl(), std::string("radio.json"), TEST_LOCATION);

  assigned = std::move(image);
  DALI_TEST_EQUALS(assigned.GetUrl(), std::string("replacement.json"), TEST_LOCATION);
  DALI_TEST_EQUALS(assigned.GetInnerFillKeyPath(), std::string("replacement.fill"), TEST_LOCATION);
  DALI_TEST_ASSERTION(image.GetUrl(), MOVED_FROM_ASSERTION);

  // Move assignment also restores a moved-from destination.
  image = std::move(copy);
  DALI_TEST_EQUALS(image.GetUrl(), std::string("radio.json"), TEST_LOCATION);
  DALI_TEST_ASSERTION(copy.GetUrl(), MOVED_FROM_ASSERTION);

  // Self-copy must preserve the owned description.
  const SelectableLottieImage& self = image;
  image = self;
  DALI_TEST_EQUALS(image.GetSelectionPlaybackRange().GetStart(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSelectableLottieImageLegacyPlaybackRanges(void)
{
  SelectableLottieImage image("checkbox.json", Int32Pair(1, 12), Int32Pair(13, 24));
  DALI_TEST_EQUALS(image.GetSelectRange().GetStart(), image.GetSelectionPlaybackRange().GetStart(), TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetSelectRange().GetEnd(), image.GetSelectionPlaybackRange().GetEnd(), TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectRange().GetStart(), image.GetDeselectionPlaybackRange().GetStart(), TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetDeselectRange().GetEnd(), image.GetDeselectionPlaybackRange().GetEnd(), TEST_LOCATION);
  END_TEST;
}
