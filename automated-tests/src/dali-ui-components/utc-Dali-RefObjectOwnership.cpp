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
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>
#include <dali-ui-components/integration-api/chart/chart-annotation-impl.h>
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-section-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/line-series-impl.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>
#include <dali-ui-components/integration-api/chart/scatter-series-impl.h>
#include <dali-ui-components/integration-api/navigator/navigation-transition-spec-impl.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
namespace Implementation = Dali::Ui::Integration;

static_assert(!std::is_default_constructible_v<Implementation::BarSeries>);
static_assert(!std::is_destructible_v<Implementation::BarSeries>);
static_assert(!std::is_copy_constructible_v<Implementation::BarSeries>);
static_assert(!std::is_copy_assignable_v<Implementation::BarSeries>);
static_assert(!std::is_move_constructible_v<Implementation::BarSeries>);
static_assert(!std::is_move_assignable_v<Implementation::BarSeries>);
static_assert(!std::is_default_constructible_v<Implementation::ChartAnnotation>);
static_assert(!std::is_destructible_v<Implementation::ChartAnnotation>);
static_assert(!std::is_copy_constructible_v<Implementation::ChartAnnotation>);
static_assert(!std::is_copy_assignable_v<Implementation::ChartAnnotation>);
static_assert(!std::is_move_constructible_v<Implementation::ChartAnnotation>);
static_assert(!std::is_move_assignable_v<Implementation::ChartAnnotation>);
static_assert(!std::is_default_constructible_v<Implementation::ChartAxis>);
static_assert(!std::is_destructible_v<Implementation::ChartAxis>);
static_assert(!std::is_copy_constructible_v<Implementation::ChartAxis>);
static_assert(!std::is_copy_assignable_v<Implementation::ChartAxis>);
static_assert(!std::is_move_constructible_v<Implementation::ChartAxis>);
static_assert(!std::is_move_assignable_v<Implementation::ChartAxis>);
static_assert(!std::is_default_constructible_v<Implementation::ChartSection>);
static_assert(!std::is_destructible_v<Implementation::ChartSection>);
static_assert(!std::is_copy_constructible_v<Implementation::ChartSection>);
static_assert(!std::is_copy_assignable_v<Implementation::ChartSection>);
static_assert(!std::is_move_constructible_v<Implementation::ChartSection>);
static_assert(!std::is_move_assignable_v<Implementation::ChartSection>);
static_assert(!std::is_default_constructible_v<Implementation::ChartSeries>);
static_assert(!std::is_destructible_v<Implementation::ChartSeries>);
static_assert(!std::is_copy_constructible_v<Implementation::ChartSeries>);
static_assert(!std::is_copy_assignable_v<Implementation::ChartSeries>);
static_assert(!std::is_move_constructible_v<Implementation::ChartSeries>);
static_assert(!std::is_move_assignable_v<Implementation::ChartSeries>);
static_assert(!std::is_default_constructible_v<Implementation::LineSeries>);
static_assert(!std::is_destructible_v<Implementation::LineSeries>);
static_assert(!std::is_copy_constructible_v<Implementation::LineSeries>);
static_assert(!std::is_copy_assignable_v<Implementation::LineSeries>);
static_assert(!std::is_move_constructible_v<Implementation::LineSeries>);
static_assert(!std::is_move_assignable_v<Implementation::LineSeries>);
static_assert(!std::is_default_constructible_v<Implementation::PieSeries>);
static_assert(!std::is_destructible_v<Implementation::PieSeries>);
static_assert(!std::is_copy_constructible_v<Implementation::PieSeries>);
static_assert(!std::is_copy_assignable_v<Implementation::PieSeries>);
static_assert(!std::is_move_constructible_v<Implementation::PieSeries>);
static_assert(!std::is_move_assignable_v<Implementation::PieSeries>);
static_assert(!std::is_default_constructible_v<Implementation::ScatterSeries>);
static_assert(!std::is_destructible_v<Implementation::ScatterSeries>);
static_assert(!std::is_copy_constructible_v<Implementation::ScatterSeries>);
static_assert(!std::is_copy_assignable_v<Implementation::ScatterSeries>);
static_assert(!std::is_move_constructible_v<Implementation::ScatterSeries>);
static_assert(!std::is_move_assignable_v<Implementation::ScatterSeries>);
static_assert(!std::is_default_constructible_v<Implementation::NavigationTransitionSpecImpl>);
static_assert(!std::is_destructible_v<Implementation::NavigationTransitionSpecImpl>);
static_assert(!std::is_copy_constructible_v<Implementation::NavigationTransitionSpecImpl>);
static_assert(!std::is_copy_assignable_v<Implementation::NavigationTransitionSpecImpl>);
static_assert(!std::is_move_constructible_v<Implementation::NavigationTransitionSpecImpl>);
static_assert(!std::is_move_assignable_v<Implementation::NavigationTransitionSpecImpl>);

namespace
{
template<typename T>
void CheckOwnership(IntrusivePtr<T> implementation)
{
  DALI_TEST_CHECK(implementation);
  DALI_TEST_EQUALS(implementation->ReferenceCount(), 1u, TEST_LOCATION);
  BaseHandle handle(implementation.Get());
  DALI_TEST_EQUALS(implementation->ReferenceCount(), 2u, TEST_LOCATION);
  implementation.Reset();
  DALI_TEST_EQUALS(handle.GetBaseObject().ReferenceCount(), 1u, TEST_LOCATION);
  handle.Reset();
}
} // namespace

int UtcDaliComponentsImplementationFactoryOwnership(void)
{
  UiTestApplication application(Components::UiConfig::New());
  CheckOwnership(Implementation::BarSeries::New());
  CheckOwnership(Implementation::ChartAnnotation::New(ChartAnnotation::Type::TEXT));
  CheckOwnership(Implementation::ChartAxis::New());
  CheckOwnership(Implementation::ChartSection::New());
  CheckOwnership(Implementation::ChartSeries::New());
  CheckOwnership(Implementation::LineSeries::New());
  CheckOwnership(Implementation::PieSeries::New());
  CheckOwnership(Implementation::ScatterSeries::New());
  CheckOwnership(Implementation::NavigationTransitionSpecImpl::New());
  END_TEST;
}
