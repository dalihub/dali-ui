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


#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/dialog/dialog-post-options.h>
#include <dali-ui-test-suite-utils.h>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr const char* MOVED_FROM_MESSAGE = "Cannot use a moved-from DialogPostOptions object";
} // namespace

int UtcDaliDialogPostOptionsDefaultsP(void)
{
  UiTestApplication application;

  DialogPostOptions options;
  DALI_TEST_CHECK(options.IsAnimated());
  DALI_TEST_CHECK(!options.GetContainerStyle());
  DALI_TEST_CHECK(!options.GetTransitionSpec());

  END_TEST;
}

int UtcDaliDialogPostOptionsSetGetP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  auto style = DialogContainerStyle::NoScrimPreset();
  auto spec  = NavigationTransitionSpec::New();

  DialogPostOptions options;
  options.SetAnimated(false);
  options.SetContainerStyle(style);
  options.SetTransitionSpec(spec);

  DALI_TEST_CHECK(!options.IsAnimated());
  DALI_TEST_CHECK(options.GetContainerStyle() == style);
  DALI_TEST_CHECK(options.GetTransitionSpec() == spec);

  options.SetAnimated(true);
  DALI_TEST_CHECK(options.IsAnimated());

  END_TEST;
}

int UtcDaliDialogPostOptionsCopyP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  auto spec = NavigationTransitionSpec::New();

  DialogPostOptions original;
  original.SetAnimated(false);
  original.SetContainerStyle(DialogContainerStyle::NoScrimPreset());
  original.SetTransitionSpec(spec);

  // Copy constructor copies values.
  DialogPostOptions copy(original);
  DALI_TEST_CHECK(!copy.IsAnimated());
  DALI_TEST_CHECK(copy.GetContainerStyle() == original.GetContainerStyle());
  DALI_TEST_CHECK(copy.GetTransitionSpec() == spec);

  // The copy is independent from the original.
  copy.SetAnimated(true);
  copy.SetTransitionSpec(NavigationTransitionSpec());
  DALI_TEST_CHECK(!original.IsAnimated());
  DALI_TEST_CHECK(original.GetTransitionSpec() == spec);

  // Copy assignment copies values and replaces the previous state.
  DialogPostOptions assigned;
  assigned = original;
  DALI_TEST_CHECK(!assigned.IsAnimated());
  DALI_TEST_CHECK(assigned.GetContainerStyle() == original.GetContainerStyle());
  DALI_TEST_CHECK(assigned.GetTransitionSpec() == spec);

  assigned.SetAnimated(true);
  DALI_TEST_CHECK(!original.IsAnimated());

  // Self assignment keeps the values.
  DialogPostOptions& self = assigned;
  assigned                = self;
  DALI_TEST_CHECK(assigned.IsAnimated());

  END_TEST;
}

int UtcDaliDialogPostOptionsMoveP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  auto spec = NavigationTransitionSpec::New();

  DialogPostOptions original;
  original.SetAnimated(false);
  original.SetTransitionSpec(spec);

  // Move constructor transfers values.
  DialogPostOptions moved(std::move(original));
  DALI_TEST_CHECK(!moved.IsAnimated());
  DALI_TEST_CHECK(moved.GetTransitionSpec() == spec);

  // Move assignment transfers values and replaces the previous state.
  DialogPostOptions target;
  target = std::move(moved);
  DALI_TEST_CHECK(!target.IsAnimated());
  DALI_TEST_CHECK(target.GetTransitionSpec() == spec);

  // Self move assignment keeps the object usable.
  DialogPostOptions& self = target;
  target                  = std::move(self);
  DALI_TEST_CHECK(!target.IsAnimated());

  // A moved-from object can be assigned again and then used.
  original = target;
  DALI_TEST_CHECK(!original.IsAnimated());
  moved = DialogPostOptions();
  DALI_TEST_CHECK(moved.IsAnimated());

  END_TEST;
}

int UtcDaliDialogPostOptionsMovedFromN(void)
{
  UiTestApplication application(Components::UiConfig::New());

  DialogPostOptions original;
  DialogPostOptions moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(original.IsAnimated(), MOVED_FROM_MESSAGE);
  DALI_TEST_ASSERTION(original.SetAnimated(false), MOVED_FROM_MESSAGE);
  DALI_TEST_ASSERTION(original.GetContainerStyle(), MOVED_FROM_MESSAGE);
  DALI_TEST_ASSERTION(original.SetContainerStyle(DialogContainerStyle()), MOVED_FROM_MESSAGE);
  DALI_TEST_ASSERTION(original.GetTransitionSpec(), MOVED_FROM_MESSAGE);
  DALI_TEST_ASSERTION(original.SetTransitionSpec(NavigationTransitionSpec()), MOVED_FROM_MESSAGE);

  END_TEST;
}

int UtcDaliDialogPostOptionsCopyFromMovedN(void)
{
  UiTestApplication application(Components::UiConfig::New());

  DialogPostOptions original;
  DialogPostOptions moved(std::move(original));
  (void)moved;

  DALI_TEST_ASSERTION(DialogPostOptions copy(original); (void)copy, MOVED_FROM_MESSAGE);

  DialogPostOptions target;
  DALI_TEST_ASSERTION(target = original, MOVED_FROM_MESSAGE);

  END_TEST;
}
