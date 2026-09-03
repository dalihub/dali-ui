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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-adaptor-impl.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-info.h>

#include <limits>
#include <string>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

namespace
{

constexpr float FIXTURE_WIDTH = 200.0f;
constexpr float FIXTURE_HEIGHT = 40.0f;

ProgressBarStyle CreateFixtureStyle()
{
  return ProgressBarStyle::Builder()
    .SetPadding(Insets(8.0f, 12.0f, 4.0f, 6.0f))
    .SetTrackThickness(10.0f)
    .SetTrackColor(UiColor(Color::RED))
    .SetTrailColor(UiColor(Color::GREEN))
    .SetDividerColor(UiColor(Color::BLUE))
    .Build();
}

UiStyle CreateProgressBarOverride()
{
  return ProgressBarStyle::Builder().SetTrackThickness(17.0f).Build();
}

void ArrangeOnScene(UiTestApplication& application, ProgressBar progressBar)
{
  progressBar.SetRequestedWidth(FIXTURE_WIDTH);
  progressBar.SetRequestedHeight(FIXTURE_HEIGHT);
  progressBar.SetPivot(Pivot::TOP_LEFT);
  progressBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  application.GetScene().Add(progressBar);
  progressBar.Measure(FIXTURE_WIDTH, FIXTURE_HEIGHT);
  progressBar.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));
  application.SendNotification();
  application.Render();
}

void Rearrange(ProgressBar progressBar)
{
  progressBar.Measure(FIXTURE_WIDTH, FIXTURE_HEIGHT);
  progressBar.Arrange(LayoutRect(0.0f, 0.0f, FIXTURE_WIDTH, FIXTURE_HEIGHT));
}

float PositionX(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_X);
}

float PositionY(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::POSITION_Y);
}

float Width(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_WIDTH);
}

float Height(Actor actor)
{
  return actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
}

bool HasIndeterminateBarWithinTrack(View track)
{
  for(uint32_t index = 1u; index < 5u; ++index)
  {
    View bar = track.GetChildViewAt(index);
    const float x = bar.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    const float width = bar.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    const float height = bar.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
    if(width > 0.0f && height > 0.0f && x < Width(track) && x + width > 0.0f)
    {
      return true;
    }
  }
  return false;
}

} // namespace

void utc_dali_progress_bar_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_progress_bar_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliProgressBarNewCopyMoveDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New();

  float minimumValue = -1.0f;
  float maximumValue = -1.0f;
  progressBar.GetRange(minimumValue, maximumValue);

  DALI_TEST_CHECK(progressBar);
  DALI_TEST_CHECK(ProgressView::DownCast(progressBar));
  DALI_TEST_EQUALS(minimumValue, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(maximumValue, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetValue(), 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(progressBar.IsDeterminate());

  ProgressBar ranged = ProgressBar::New(-10.0f, 30.0f);
  ranged.GetRange(minimumValue, maximumValue);
  DALI_TEST_EQUALS(minimumValue, -10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(maximumValue, 30.0f, TEST_LOCATION);

  ProgressBar copy(progressBar);
  DALI_TEST_CHECK(copy == progressBar);
  ProgressBar moved(std::move(copy));
  DALI_TEST_CHECK(moved == progressBar);
  DALI_TEST_CHECK(ProgressBar::DownCast(BaseHandle(progressBar)) == progressBar);
  DALI_TEST_CHECK(!ProgressBar::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!ProgressBar::DownCast(View::New()));

  TypeInfo progressBarType;
  DALI_TEST_CHECK(progressBar.GetTypeInfo(progressBarType));
  DALI_TEST_CHECK(!progressBarType.CreateInstance());

  DALI_TEST_ASSERTION(ProgressBar::New(1.0f, 1.0f), "maximum must exceed minimum");
  DALI_TEST_ASSERTION(ProgressBar::New(ProgressBarStyle()), "must be initialized");
  END_TEST;
}

int UtcDaliProgressBarStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());

  ProgressBarStyle emptyStyle;
  DALI_TEST_CHECK(!emptyStyle);

  ProgressBarStyle defaultStyle = ProgressBarStyle::Default();
  DALI_TEST_CHECK(defaultStyle);
  DALI_TEST_CHECK(ProgressBarStyle::DefaultPreset());
  DALI_TEST_CHECK(ProgressBarStyle::DefaultKey() == ProgressBarStyle::DefaultKey());
  DALI_TEST_EQUALS(defaultStyle.GetPadding(), Insets(0.0f, 0.0f, 10.0f, 10.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(defaultStyle.GetTrackThickness(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultStyle.GetTrackColor().GetRgba(), UiColor(0x1C1C1Eu, 0.1f).GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(defaultStyle.GetTrailColor().GetRgba(), UiColor::PRIMARY.GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(defaultStyle.GetDividerColor().GetRgba(), UiColor(0xFFFFFFu, 0.6f).GetRgba(), TEST_LOCATION);

  ProgressBarStyle fixtureStyle = CreateFixtureStyle();
  DALI_TEST_EQUALS(fixtureStyle.GetPadding(), Insets(8.0f, 12.0f, 4.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(fixtureStyle.GetTrackThickness(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(fixtureStyle.GetTrackColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(fixtureStyle.GetTrailColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(fixtureStyle.GetDividerColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  ProgressBarStyle configured = fixtureStyle.Configure().SetTrackThickness(13.0f).Build();
  DALI_TEST_EQUALS(configured.GetTrackThickness(), 13.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(fixtureStyle.GetTrackThickness(), 10.0f, TEST_LOCATION);

  ProgressBarStyle::Builder builder;
  ProgressBarStyle built = std::move(builder).Build();
  DALI_TEST_CHECK(built);
  DALI_TEST_ASSERTION(builder.SetTrackThickness(2.0f), "already been consumed");

  ProgressBarStyle::Builder moveSource;
  ProgressBarStyle::Builder moveConstructed(std::move(moveSource));
  ProgressBarStyle::Builder moveAssigned;
  moveAssigned = std::move(moveConstructed);
  ProgressBarStyle movedStyle = std::move(moveAssigned).Build();
  DALI_TEST_CHECK(movedStyle);

  DALI_TEST_ASSERTION(ProgressBarStyle::Builder().SetTrackThickness(0.0f), "thickness must be finite and positive");
  DALI_TEST_ASSERTION(ProgressBarStyle::Builder().SetPadding(Insets(-1.0f, 0.0f, 0.0f, 0.0f)), "padding must be finite and non-negative");
  END_TEST;
}

int UtcDaliProgressBarStyleDefaultKeyOverrideP(void)
{
  Components::UiConfig config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(ProgressBarStyle::DefaultKey(), &CreateProgressBarOverride);
  UiTestApplication application(config);

  ProgressBarStyle style = ProgressBarStyle::Default();
  ProgressBar progressBar = ProgressBar::New();
  DALI_TEST_EQUALS(style.GetTrackThickness(), 17.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrackThickness(), 17.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarLayoutAndRuntimeStyleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.25f);
  progressBar.SetDividerStepCount(4);
  ArrangeOnScene(application, progressBar);

  DALI_TEST_EQUALS(progressBar.GetPadding(), Insets(8.0f, 12.0f, 4.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrackThickness(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrackColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrailColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetChildCount(), 1u, TEST_LOCATION);

  View track = progressBar.GetChildViewAt(0u);
  View trail = track.GetChildViewAt(0u);
  DALI_TEST_EQUALS(PositionX(track), 8.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionY(track), 14.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Width(track), 180.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Height(track), 10.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(PositionX(trail), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Width(trail), 45.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(track.IsAccessibilityHidden());
  DALI_TEST_CHECK(trail.IsAccessibilityHidden());
  DALI_TEST_EQUALS(track.GetBackgroundColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(trail.GetBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  progressBar.SetTrackThickness(8.0f);
  progressBar.SetTrackColor(UiColor(Color::YELLOW));
  progressBar.SetTrailColor(UiColor(Color::CYAN));
  progressBar.SetTrackThickness(8.0f);
  progressBar.SetTrackColor(UiColor(Color::YELLOW));
  progressBar.SetTrailColor(UiColor(Color::CYAN));
  Rearrange(progressBar);
  DALI_TEST_EQUALS(progressBar.GetTrackThickness(), 8.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrackColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrailColor().GetRgba(), Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetBackgroundColor().GetRgba(), Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(trail.GetBackgroundColor().GetRgba(), Color::CYAN, TEST_LOCATION);

  DALI_TEST_ASSERTION(progressBar.SetTrackThickness(0.0f), "thickness must be finite and positive");
  DALI_TEST_ASSERTION(progressBar.SetTrackThickness(std::numeric_limits<float>::infinity()), "thickness must be finite and positive");
  DALI_TEST_EQUALS(progressBar.GetTrackThickness(), 8.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarDividerContractP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.25f);
  progressBar.SetDividerStepCount(4);
  ArrangeOnScene(application, progressBar);

  const uint32_t dividerChildCount = progressBar.GetChildViewAt(0u).GetChildCount();
  progressBar.SetDividerStepCount(4);
  DALI_TEST_EQUALS(progressBar.GetChildViewAt(0u).GetChildCount(), dividerChildCount, TEST_LOCATION);

  DALI_TEST_EQUALS(progressBar.GetDividerStepCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), Vector2(53.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(1u), Vector2(98.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(2u), Vector2(143.0f, 19.0f), TEST_LOCATION);

  View track = progressBar.GetChildViewAt(0u);
  DALI_TEST_EQUALS(track.GetChildCount(), 8u, TEST_LOCATION);
  for(uint32_t index = 1u; index < 5u; ++index)
  {
    DALI_TEST_CHECK(!track.GetChildViewAt(index).IsVisible());
    DALI_TEST_CHECK(track.GetChildViewAt(index).IsAccessibilityHidden());
  }
  for(uint32_t index = 5u; index < track.GetChildCount(); ++index)
  {
    DALI_TEST_CHECK(track.GetChildViewAt(index).IsAccessibilityHidden());
    DALI_TEST_EQUALS(track.GetChildViewAt(index).GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  }

  progressBar.SetDividerStepCount(1);
  DALI_TEST_EQUALS(progressBar.GetDividerCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetChildCount(), 5u, TEST_LOCATION);
  DALI_TEST_ASSERTION(progressBar.GetDividerPosition(0u), "divider index is out of range");
  DALI_TEST_ASSERTION(progressBar.SetDividerStepCount(-1), "divider step count must be non-negative");
  DALI_TEST_EQUALS(progressBar.GetDividerStepCount(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarGeometrySignalsAndDirectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetDividerStepCount(4);
  ArrangeOnScene(application, progressBar);
  application.RunIdles();

  int trailSignalCount = 0;
  int dividerSignalCount = 0;
  int valueSignalCount = 0;
  ProgressBar trailSender;
  ProgressBar dividerSender;
  Vector2 expectedTrailPosition;
  Vector2 expectedFirstDividerPosition;
  uint32_t expectedDividerCount = 0u;
  std::string notificationOrder;
  progressBar.ValueChangedSignal().Connect(&application, [&](ProgressView sender, float)
  {
    ++valueSignalCount;
    ProgressBar valueSender = ProgressBar::DownCast(sender);
    DALI_TEST_CHECK(valueSender == progressBar);
    DALI_TEST_EQUALS(valueSender.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  });
  progressBar.TrailEndPositionChangedSignal().Connect(&application, [&](ProgressBar sender)
  {
    ++trailSignalCount;
    trailSender = sender;
    notificationOrder += "T";
    DALI_TEST_EQUALS(sender.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  });
  progressBar.DividersChangedSignal().Connect(&application, [&](ProgressBar sender)
  {
    ++dividerSignalCount;
    dividerSender = sender;
    notificationOrder += "D";
    DALI_TEST_EQUALS(sender.GetDividerCount(), expectedDividerCount, TEST_LOCATION);
    if(expectedDividerCount > 0u)
    {
      DALI_TEST_EQUALS(sender.GetDividerPosition(0u), expectedFirstDividerPosition, TEST_LOCATION);
    }
  });

  expectedTrailPosition = Vector2(53.0f, 19.0f);
  progressBar.SetValue(0.25f);
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(trailSignalCount, 0, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(trailSender == progressBar);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);

  expectedTrailPosition = Vector2(98.0f, 19.0f);
  progressBar.SetValue(0.5f);
  expectedTrailPosition = Vector2(143.0f, 19.0f);
  progressBar.SetValue(0.75f);
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 2, TEST_LOCATION);

  expectedTrailPosition = Vector2(53.0f, 19.0f);
  progressBar.SetReversed(true);
  DALI_TEST_CHECK(progressBar.IsReversed());
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);

  progressBar.SetReversed(true);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);

  notificationOrder.clear();
  expectedTrailPosition = Vector2(147.0f, 19.0f);
  expectedDividerCount = 3u;
  expectedFirstDividerPosition = Vector2(147.0f, 19.0f);
  progressBar.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  Rearrange(progressBar);
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), expectedTrailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), Vector2(147.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(1u), Vector2(102.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(2u), Vector2(57.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(dividerSender == progressBar);
  DALI_TEST_EQUALS(notificationOrder, std::string("TD"), TEST_LOCATION);

  notificationOrder.clear();
  expectedDividerCount = 4u;
  expectedFirstDividerPosition = Vector2(156.0f, 19.0f);
  progressBar.SetDividerStepCount(5);
  DALI_TEST_EQUALS(progressBar.GetDividerCount(), 4u, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(dividerSignalCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(notificationOrder, std::string("D"), TEST_LOCATION);

  progressBar.SetTrackColor(UiColor(Color::MAGENTA));
  progressBar.SetTrailColor(UiColor(Color::BLACK));
  progressBar.SetDeterminate(false);
  progressBar.SetDeterminate(true);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(valueSignalCount, 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarZeroSizeAndSceneGeometryP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.5f);
  progressBar.SetDividerStepCount(4);
  progressBar.Measure(0.0f, 0.0f);
  progressBar.Arrange(LayoutRect(0.0f, 0.0f, 0.0f, 0.0f));

  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), Vector2(8.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerCount(), 3u, TEST_LOCATION);
  for(uint32_t index = 0u; index < progressBar.GetDividerCount(); ++index)
  {
    DALI_TEST_EQUALS(progressBar.GetDividerPosition(index), Vector2(8.0f, 4.0f), TEST_LOCATION);
  }

  ArrangeOnScene(application, progressBar);
  application.RunIdles();
  const Vector2 trailPosition = progressBar.GetTrailEndPosition();
  const Vector2 dividerPosition = progressBar.GetDividerPosition(0u);
  DALI_TEST_EQUALS(trailPosition, Vector2(98.0f, 19.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(dividerPosition, Vector2(53.0f, 19.0f), TEST_LOCATION);

  int trailSignalCount = 0;
  int dividerSignalCount = 0;
  progressBar.TrailEndPositionChangedSignal().Connect(&application, [&](ProgressBar)
  {
    ++trailSignalCount;
  });
  progressBar.DividersChangedSignal().Connect(&application, [&](ProgressBar)
  {
    ++dividerSignalCount;
  });

  application.GetScene().Remove(progressBar);
  application.SendNotification();
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), trailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), dividerPosition, TEST_LOCATION);

  progressBar.SetDeterminate(false);
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), trailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), dividerPosition, TEST_LOCATION);

  application.GetScene().Add(progressBar);
  application.SendNotification();
  application.RunIdles();
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), trailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), dividerPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(trailSignalCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);

  application.GetScene().Remove(progressBar);
  application.SendNotification();
  END_TEST;
}

int UtcDaliProgressBarPendingGeometryCallbackLifetimeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  int signalCount = 0;

  {
    ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
    progressBar.TrailEndPositionChangedSignal().Connect(&application, [&](ProgressBar)
    {
      ++signalCount;
    });
    progressBar.DividersChangedSignal().Connect(&application, [&](ProgressBar)
    {
      ++signalCount;
    });
    progressBar.SetDividerStepCount(4);
  }

  application.RunIdles();
  DALI_TEST_EQUALS(signalCount, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarGeometryNotificationFailureP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.25f);
  ArrangeOnScene(application, progressBar);
  application.RunIdles();

  int signalCount = 0;
  progressBar.TrailEndPositionChangedSignal().Connect(&application, [&](ProgressBar)
  {
    ++signalCount;
  });

  UiTestApplication::ADD_IDLE_SUCCESS = false;
  progressBar.SetReversed(true);
  UiTestApplication::ADD_IDLE_SUCCESS = true;
  application.RunIdles();
  DALI_TEST_EQUALS(signalCount, 0, TEST_LOCATION);

  progressBar.SetReversed(false);
  application.RunIdles();
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);

  Test::UiAdaptor::SetAdaptorAvailableForce(false);
  progressBar.SetReversed(true);
  Test::UiAdaptor::SetAdaptorAvailableForce(true);
  application.RunIdles();
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);

  progressBar.SetReversed(false);
  application.RunIdles();
  DALI_TEST_EQUALS(signalCount, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarIndeterminateResizeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.5f);
  progressBar.SetDividerStepCount(4);
  ArrangeOnScene(application, progressBar);
  application.RunIdles();

  View track = progressBar.GetChildViewAt(0u);
  progressBar.SetDeterminate(false);
  application.SendNotification();
  application.Render(100);

  constexpr float resizedWidth = 260.0f;
  progressBar.SetRequestedWidth(resizedWidth);
  progressBar.Measure(resizedWidth, FIXTURE_HEIGHT);
  progressBar.Arrange(LayoutRect(0.0f, 0.0f, resizedWidth, FIXTURE_HEIGHT));
  application.SendNotification();
  application.Render(100);

  DALI_TEST_EQUALS(Width(track), 240.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::REFRESH_ALWAYS),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetChildCount(), 9u, TEST_LOCATION);
  DALI_TEST_CHECK(HasIndeterminateBarWithinTrack(track));

  progressBar.SetDeterminate(true);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::NONE),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetChildCount(), 8u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarDeterminateLifecycleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New(CreateFixtureStyle());
  progressBar.SetValue(0.5f);
  progressBar.SetDividerStepCount(4);
  ArrangeOnScene(application, progressBar);
  application.RunIdles();
  const Vector2 trailPosition = progressBar.GetTrailEndPosition();
  const Vector2 dividerPosition = progressBar.GetDividerPosition(0u);

  int trailSignalCount = 0;
  int dividerSignalCount = 0;
  progressBar.TrailEndPositionChangedSignal().Connect(&application, [&](ProgressBar)
  {
    ++trailSignalCount;
  });
  progressBar.DividersChangedSignal().Connect(&application, [&](ProgressBar)
  {
    ++dividerSignalCount;
  });

  View track = progressBar.GetChildViewAt(0u);
  const uint32_t componentChildCount = 8u;
  const uint32_t offScreenChildCount = componentChildCount + 1u;
  DALI_TEST_EQUALS(track.GetChildCount(), componentChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::NONE),
                   TEST_LOCATION);

  progressBar.SetDeterminate(false);
  DALI_TEST_EQUALS(track.GetChildCount(), offScreenChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::REFRESH_ALWAYS),
                   TEST_LOCATION);
  DALI_TEST_CHECK(!track.GetChildViewAt(0u).IsVisible());
  for(uint32_t index = 5u; index < componentChildCount; ++index)
  {
    DALI_TEST_CHECK(!track.GetChildViewAt(index).IsVisible());
  }
  for(uint32_t index = 1u; index < 5u; ++index)
  {
    DALI_TEST_CHECK(track.GetChildViewAt(index).IsAccessibilityHidden());
    DALI_TEST_CHECK(track.GetChildViewAt(index).GetLayoutMode() == LayoutMode::DEFAULT);
    DALI_TEST_CHECK(track.GetChildViewAt(index).IsVisible());
  }
  application.SendNotification();
  application.Render(0);
  for(uint32_t index = 1u; index < 5u; ++index)
  {
    DALI_TEST_CHECK(track.GetChildViewAt(index).GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH) > 0.0f);
    DALI_TEST_CHECK(track.GetChildViewAt(index).GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT) > 0.0f);
  }
  application.Render(900);

  DALI_TEST_CHECK(HasIndeterminateBarWithinTrack(track));
  progressBar.SetDeterminate(true);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::NONE),
                   TEST_LOCATION);
  progressBar.SetDeterminate(false);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::REFRESH_ALWAYS),
                   TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetChildCount(), offScreenChildCount, TEST_LOCATION);
  for(uint32_t frame = 0u; frame < 9u; ++frame)
  {
    Rearrange(progressBar);
    application.SendNotification();
    application.Render(100);
  }
  DALI_TEST_CHECK(HasIndeterminateBarWithinTrack(track));
  DALI_TEST_EQUALS(track.GetChildCount(), offScreenChildCount, TEST_LOCATION);
  application.RunIdles();
  DALI_TEST_EQUALS(trailSignalCount, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(dividerSignalCount, 0, TEST_LOCATION);

  progressBar.SetDeterminate(true);
  DALI_TEST_EQUALS(track.GetChildCount(), componentChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::NONE),
                   TEST_LOCATION);
  DALI_TEST_CHECK(track.GetChildViewAt(0u).IsVisible());
  for(uint32_t index = 1u; index < 5u; ++index)
  {
    DALI_TEST_CHECK(!track.GetChildViewAt(index).IsVisible());
  }
  for(uint32_t index = 5u; index < componentChildCount; ++index)
  {
    DALI_TEST_CHECK(track.GetChildViewAt(index).IsVisible());
  }

  application.GetScene().Remove(progressBar);
  application.SendNotification();
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), trailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), dividerPosition, TEST_LOCATION);
  progressBar.SetDeterminate(false);
  DALI_TEST_EQUALS(track.GetChildCount(), componentChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(track.GetProperty<int32_t>(View::Property::OFFSCREEN_RENDERING),
                   static_cast<int32_t>(View::OffScreenRenderingType::REFRESH_ALWAYS),
                   TEST_LOCATION);
  application.GetScene().Add(progressBar);
  application.SendNotification();
  DALI_TEST_EQUALS(track.GetChildCount(), offScreenChildCount, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetTrailEndPosition(), trailPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(progressBar.GetDividerPosition(0u), dividerPosition, TEST_LOCATION);
  application.GetScene().Remove(progressBar);
  application.SendNotification();
  DALI_TEST_EQUALS(track.GetChildCount(), componentChildCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressBarAccessibilityP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ProgressBar progressBar = ProgressBar::New();
  auto accessible = Dali::Accessibility::Accessible::Get(progressBar);

  DALI_TEST_CHECK(accessible);
  DALI_TEST_CHECK(progressBar.GetAccessibilityRole() == UiAccessibility::Role::PROGRESS_BAR);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PROGRESS_BAR, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string("0%"), TEST_LOCATION);
  DALI_TEST_CHECK(progressBar.GetChildViewAt(0u).IsAccessibilityHidden());
  END_TEST;
}
