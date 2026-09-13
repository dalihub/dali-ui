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

#include <dali-ui-foundation/extension-api/progress-view-impl.h>
#include <dali-ui-foundation/public-api/views/progress-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-info.h>
#include <dali/devel-api/object/type-registry.h>

#include <limits>
#include <string>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace UiAccessibility = Dali::Ui::Accessibility;

namespace
{

class TestProgressViewImpl : public Extension::ProgressViewImpl
{
public:
  static IntrusivePtr<TestProgressViewImpl> New(float minimumValue, float maximumValue)
  {
    return new TestProgressViewImpl(minimumValue, maximumValue);
  }

  int GetRatioHookCount() const
  {
    return mRatioHookCount;
  }

  float GetLastRatio() const
  {
    return mLastRatio;
  }

  int GetDeterminateHookCount() const
  {
    return mDeterminateHookCount;
  }

  bool GetLastDeterminate() const
  {
    return mLastDeterminate;
  }

protected:
  TestProgressViewImpl(float minimumValue, float maximumValue)
  : ProgressViewImpl(minimumValue, maximumValue)
  {
  }

  void OnValueRatioChanged(float valueRatio) override
  {
    ++mRatioHookCount;
    mLastRatio = valueRatio;
  }

  void OnDeterminateChanged(bool determinate) override
  {
    ++mDeterminateHookCount;
    mLastDeterminate = determinate;
  }

private:
  int mRatioHookCount{0};
  float mLastRatio{0.0f};
  int mDeterminateHookCount{0};
  bool mLastDeterminate{true};
};

struct ProgressViewFixture
{
  ProgressViewFixture(float minimumValue = 0.0f, float maximumValue = 100.0f)
  : impl(TestProgressViewImpl::New(minimumValue, maximumValue)),
    view(*impl)
  {
    impl->Initialize();
  }

  IntrusivePtr<TestProgressViewImpl> impl;
  ProgressView view;
};

} // namespace

void utc_dali_progress_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_progress_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliProgressViewNewCopyMoveDownCastP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture(-10.0f, 30.0f);

  float minimumValue = 0.0f;
  float maximumValue = 0.0f;
  fixture.view.GetRange(minimumValue, maximumValue);

  DALI_TEST_EQUALS(minimumValue, -10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(maximumValue, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.view.GetValue(), -10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.view.GetValueStepCount(), 0, TEST_LOCATION);
  DALI_TEST_CHECK(fixture.view.IsDeterminate());
  DALI_TEST_CHECK(fixture.view.GetAccessibilityRole() == UiAccessibility::Role::PROGRESS_BAR);
  DALI_TEST_EQUALS(fixture.impl->GetRatioHookCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetDeterminateHookCount(), 0, TEST_LOCATION);

  ProgressView copy(fixture.view);
  DALI_TEST_CHECK(copy == fixture.view);
  ProgressView moved(std::move(copy));
  DALI_TEST_CHECK(moved == fixture.view);
  DALI_TEST_CHECK(ProgressView::DownCast(BaseHandle(fixture.view)) == fixture.view);
  DALI_TEST_CHECK(!ProgressView::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!ProgressView::DownCast(View::New()));

  TypeInfo progressViewType = TypeRegistry::Get().GetTypeInfo(Dali::String("ProgressView"));
  DALI_TEST_CHECK(progressViewType);
  DALI_TEST_CHECK(!progressViewType.CreateInstance());

  DALI_TEST_ASSERTION(TestProgressViewImpl::New(1.0f, 1.0f), "maximum must exceed minimum");
  DALI_TEST_ASSERTION(TestProgressViewImpl::New(std::numeric_limits<float>::quiet_NaN(), 1.0f), "range endpoints must be finite");
  END_TEST;
}

int UtcDaliProgressViewValueClampSignalAndHookP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;
  int signalCount = 0;
  float signalValue = 0.0f;
  ProgressView signalSender;

  fixture.view.ValueChangedSignal().Connect(&application, [&](ProgressView sender, float value)
  {
    ++signalCount;
    signalSender = sender;
    signalValue = value;
    DALI_TEST_EQUALS(fixture.impl->GetRatioHookCount(), signalCount, TEST_LOCATION);
  });

  fixture.view.SetValue(25.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signalValue, 25.0f, TEST_LOCATION);
  DALI_TEST_CHECK(signalSender == fixture.view);
  DALI_TEST_EQUALS(fixture.impl->GetLastRatio(), 0.25f, 0.001f, TEST_LOCATION);

  fixture.view.SetValue(25.0f);
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetRatioHookCount(), 1, TEST_LOCATION);

  fixture.view.SetValue(200.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(signalCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetLastRatio(), 1.0f, TEST_LOCATION);

  fixture.view.SetValue(-20.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(signalCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetLastRatio(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressViewRangeTransactionP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;
  int signalCount = 0;
  fixture.view.ValueChangedSignal().Connect(&application, [&](ProgressView, float)
  {
    ++signalCount;
  });

  fixture.view.SetValue(50.0f);
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);

  fixture.view.SetRange(0.0f, 200.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetRatioHookCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetLastRatio(), 0.25f, 0.001f, TEST_LOCATION);

  fixture.view.SetRange(60.0f, 160.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(signalCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetLastRatio(), 0.0f, TEST_LOCATION);

  const int ratioHookCount = fixture.impl->GetRatioHookCount();
  fixture.view.SetRange(60.0f, 160.0f);
  DALI_TEST_EQUALS(signalCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.impl->GetRatioHookCount(), ratioHookCount, TEST_LOCATION);

  DALI_TEST_ASSERTION(fixture.view.SetRange(5.0f, 5.0f), "maximum must exceed minimum");
  DALI_TEST_ASSERTION(fixture.view.SetRange(std::numeric_limits<float>::quiet_NaN(), 10.0f), "range endpoints must be finite");

  float minimumValue = 0.0f;
  float maximumValue = 0.0f;
  fixture.view.GetRange(minimumValue, maximumValue);
  DALI_TEST_EQUALS(minimumValue, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(maximumValue, 160.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 60.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressViewStepCountP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;

  fixture.view.SetValueStepCount(0);
  DALI_TEST_EQUALS(fixture.view.GetValueStepCount(), 0, TEST_LOCATION);

  fixture.view.SetValue(24.0f);
  fixture.view.SetValueStepCount(4);
  DALI_TEST_EQUALS(fixture.view.GetValueStepCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 25.0f, TEST_LOCATION);

  fixture.view.SetValue(37.5f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 50.0f, TEST_LOCATION);
  fixture.view.SetValue(-100.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 0.0f, TEST_LOCATION);
  fixture.view.SetValue(1000.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 100.0f, TEST_LOCATION);

  fixture.view.SetValueStepCount(1);
  fixture.view.SetValue(49.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 0.0f, TEST_LOCATION);
  fixture.view.SetValue(50.0f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 100.0f, TEST_LOCATION);

  fixture.view.SetValueStepCount(0);
  fixture.view.SetValue(12.5f);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 12.5f, TEST_LOCATION);
  DALI_TEST_ASSERTION(fixture.view.SetValueStepCount(-1), "step count must be non-negative");
  DALI_TEST_EQUALS(fixture.view.GetValueStepCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.view.GetValue(), 12.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressViewInvalidValueN(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;

  fixture.view.SetValue(30.0f);
  DALI_TEST_ASSERTION(fixture.view.SetValue(std::numeric_limits<float>::quiet_NaN()), "value must be finite");
  DALI_TEST_ASSERTION(fixture.view.SetValue(std::numeric_limits<float>::infinity()), "value must be finite");
  DALI_TEST_EQUALS(fixture.view.GetValue(), 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliProgressViewDeterminateSignalAndHookP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;
  int signalCount = 0;
  bool signalValue = true;

  fixture.view.DeterminateChangedSignal().Connect(&application, [&](ProgressView sender, bool determinate)
  {
    ++signalCount;
    signalValue = determinate;
    DALI_TEST_CHECK(sender == fixture.view);
    DALI_TEST_CHECK(sender.IsDeterminate() == determinate);
    DALI_TEST_EQUALS(fixture.impl->GetDeterminateHookCount(), signalCount, TEST_LOCATION);
  });

  fixture.view.SetDeterminate(false);
  DALI_TEST_CHECK(!fixture.view.IsDeterminate());
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!signalValue);
  DALI_TEST_CHECK(!fixture.impl->GetLastDeterminate());

  fixture.view.SetDeterminate(false);
  DALI_TEST_EQUALS(signalCount, 1, TEST_LOCATION);

  fixture.view.SetDeterminate(true);
  DALI_TEST_CHECK(fixture.view.IsDeterminate());
  DALI_TEST_EQUALS(signalCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(signalValue);
  DALI_TEST_CHECK(fixture.impl->GetLastDeterminate());
  END_TEST;
}

int UtcDaliProgressViewAccessibilityP(void)
{
  UiTestApplication application;
  ProgressViewFixture fixture;
  auto accessible = Dali::Accessibility::Accessible::Get(fixture.view);

  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetRole(), Dali::Integration::Accessibility::Role::PROGRESS_BAR, TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string("0%"), TEST_LOCATION);

  fixture.view.SetValue(24.6f);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string("25%"), TEST_LOCATION);

  fixture.view.SetAccessibilityValue("Loading files");
  fixture.view.SetValue(50.0f);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string("Loading files"), TEST_LOCATION);

  fixture.view.SetAccessibilityValue("");
  fixture.view.SetDeterminate(false);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string(), TEST_LOCATION);

  Property::Map actionAttributes;
  DALI_TEST_CHECK(!fixture.view.DoAction("increment", actionAttributes));
  DALI_TEST_CHECK(!fixture.view.DoAction("decrement", actionAttributes));
  END_TEST;
}
