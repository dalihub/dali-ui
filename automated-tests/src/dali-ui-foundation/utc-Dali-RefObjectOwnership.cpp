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

// EXTERNAL INCLUDES
#include <type_traits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/edge-effect-impl.h>
#include <dali-ui-foundation/integration-api/media-view-impl.h>
#include <dali-ui-foundation/integration-api/recycler/items-layouter-impl.h>
#include <dali-ui-foundation/integration-api/selectable-lottie-animation-view-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
namespace Implementation = Dali::Ui::Integration;

static_assert(!std::is_default_constructible_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_destructible_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_copy_constructible_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_copy_assignable_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_move_constructible_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_move_assignable_v<Implementation::EdgeEffectImpl>);
static_assert(!std::is_default_constructible_v<Implementation::MediaViewImpl>);
static_assert(!std::is_destructible_v<Implementation::MediaViewImpl>);
static_assert(!std::is_copy_constructible_v<Implementation::MediaViewImpl>);
static_assert(!std::is_copy_assignable_v<Implementation::MediaViewImpl>);
static_assert(!std::is_move_constructible_v<Implementation::MediaViewImpl>);
static_assert(!std::is_move_assignable_v<Implementation::MediaViewImpl>);
static_assert(!std::is_default_constructible_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_destructible_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_copy_constructible_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_copy_assignable_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_move_constructible_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_move_assignable_v<Implementation::ItemsLayouterImpl>);
static_assert(!std::is_default_constructible_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_destructible_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_copy_constructible_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_copy_assignable_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_move_constructible_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_move_assignable_v<Implementation::SelectableLottieAnimationViewImpl>);
static_assert(!std::is_destructible_v<Dali::Ui::Internal::VisualBaseImpl>);
static_assert(!std::is_copy_constructible_v<Dali::Ui::Internal::VisualBaseImpl>);
static_assert(!std::is_copy_assignable_v<Dali::Ui::Internal::VisualBaseImpl>);
static_assert(!std::is_move_constructible_v<Dali::Ui::Internal::VisualBaseImpl>);
static_assert(!std::is_move_assignable_v<Dali::Ui::Internal::VisualBaseImpl>);

int UtcDaliSelectableLottieImplementationFactoryOwnership(void)
{
  UiTestApplication application;
  auto              implementation = Implementation::SelectableLottieAnimationViewImpl::New(
    "checkbox.json", Int32Pair(0, 30), Int32Pair(30, 48));
  DALI_TEST_CHECK(implementation);
  DALI_TEST_EQUALS(implementation->ReferenceCount(), 1u, TEST_LOCATION);
  BaseHandle owner(implementation.Get());
  DALI_TEST_EQUALS(implementation->ReferenceCount(), 2u, TEST_LOCATION);
  implementation.Reset();
  DALI_TEST_EQUALS(owner.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);
  owner.Reset();

  SelectableLottieColorBindings bindings;
  auto                          explicitBindings = Implementation::SelectableLottieAnimationViewImpl::New(
    "", Int32Pair(0, 30), Int32Pair(30, 48), bindings);
  DALI_TEST_CHECK(explicitBindings);
  DALI_TEST_EQUALS(explicitBindings->ReferenceCount(), 1u, TEST_LOCATION);
  END_TEST;
}
