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

#include "manual-test-case.h"

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <algorithm>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float PANEL_HEIGHT = 88.0f;
constexpr float BAR_Y = 24.0f;
constexpr float BAR_HEIGHT = 40.0f;
constexpr uint32_t COLOR_BODY = 0x5F6368u;
constexpr uint32_t COLOR_STATUS = 0x137333u;

Label MakeLabel(const Dali::String& text, float fontSize = 14.0f, uint32_t color = COLOR_BODY)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(WRAP_CONTENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}

TextButton MakeButton(const Dali::String& text)
{
  TextButton button = TextButton::New(text);
  button.SetRequestedWidth(WRAP_CONTENT);
  button.SetRequestedHeight(42.0f);
  return button;
}

StackLayout MakeButtonRow()
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(46.0f);
  row.SetSpacing(8.0f);
  return row;
}
} // namespace

class TcProgressBarBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "ProgressBar: values, geometry and modes";
  }

  Dali::String GetDescription() const override
  {
    return "Compare default/custom appearance; select 0/25/50/100%, then toggle reverse, RTL "
           "and determinate state. The tooltip and divider labels use only public geometry APIs.";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(WRAP_CONTENT);
    root.SetSpacing(10.0f);
    root.SetPadding(Insets(16.0f, 16.0f, 12.0f, 12.0f));

    root.Add(MakeLabel("Default style at 25%"));
    ProgressBar defaultBar = ProgressBar::New();
    defaultBar.SetRequestedWidth(MATCH_PARENT);
    defaultBar.SetRequestedHeight(BAR_HEIGHT);
    defaultBar.SetValue(0.25f);
    root.Add(defaultBar);

    root.Add(MakeLabel("Custom style with public tooltip/divider synchronization"));
    ProgressBarStyle customStyle = ProgressBarStyle::Default()
                                       .Configure()
                                       .SetPadding(Insets(8.0f, 8.0f, 12.0f, 12.0f))
                                       .SetTrackThickness(14.0f)
                                       .SetTrackColor(UiColor(0xD5D9E0u))
                                       .SetTrailColor(UiColor(0x6750A4u))
                                       .SetDividerColor(UiColor(0xFFFFFFu, 0.8f))
                                       .Build();

    mOverlay = AbsoluteLayout::New();
    mOverlay.SetRequestedWidth(MATCH_PARENT);
    mOverlay.SetRequestedHeight(PANEL_HEIGHT);

    mProgress = ProgressBar::New(0.0f, 100.0f, customStyle);
    mProgress.SetRequestedWidth(MATCH_PARENT);
    mProgress.SetRequestedHeight(BAR_HEIGHT);
    mProgress.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, BAR_Y, -1.0f, BAR_HEIGHT)));
    mProgress.SetDividerStepCount(4);
    mProgress.SetValue(50.0f);
    mProgress.SetAccessibilityName("Download progress");

    mTooltip = MakeLabel("50%", 13.0f, COLOR_STATUS);

    mOverlay.Add(mProgress);
    mOverlay.Add(mTooltip);
    root.Add(mOverlay);

    mStatus = MakeLabel("value 50%, determinate, normal, LTR", 14.0f, COLOR_STATUS);
    mStatus.SetRequestedWidth(MATCH_PARENT);
    root.Add(mStatus);

    StackLayout values = MakeButtonRow();
    AddValueButton(values, "0%", 0.0f);
    AddValueButton(values, "25%", 25.0f);
    AddValueButton(values, "50%", 50.0f);
    AddValueButton(values, "100%", 100.0f);
    root.Add(values);

    StackLayout modes = MakeButtonRow();
    TextButton determinate = MakeButton("Determinate / Indeterminate");
    determinate.ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mProgress.SetDeterminate(!mProgress.IsDeterminate());
    });
    modes.Add(determinate);

    TextButton reverse = MakeButton("Reverse");
    reverse.ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mProgress.SetReversed(!mProgress.IsReversed());
      UpdateStatus();
    });
    modes.Add(reverse);

    TextButton rtl = MakeButton("RTL / LTR");
    rtl.ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mRightToLeft = !mRightToLeft;
      mProgress.SetLayoutDirection(mRightToLeft ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
      UpdateStatus();
    });
    modes.Add(rtl);
    root.Add(modes);

    mProgress.ValueChangedSignal().Connect(this, &TcProgressBarBasics::OnValueChanged);
    mProgress.DeterminateChangedSignal().Connect(this, &TcProgressBarBasics::OnDeterminateChanged);
    mProgress.TrailEndPositionChangedSignal().Connect(this, &TcProgressBarBasics::OnTrailEndPositionChanged);
    mProgress.DividersChangedSignal().Connect(this, &TcProgressBarBasics::OnDividersChanged);

    SyncTrail(mProgress);
    SyncDividers(mProgress);
    UpdateStatus();
    contentArea.Add(root);
  }

  void OnExit() override
  {
    mDividerLabels.clear();
    mTooltip.Reset();
    mStatus.Reset();
    mProgress.Reset();
    mOverlay.Reset();
  }

private:
  void AddValueButton(StackLayout row, const Dali::String& text, float value)
  {
    TextButton button = MakeButton(text);
    button.ClickedSignal().Connect(this, [this, value](View, InputEvent)
    {
      mProgress.SetValue(value);
    });
    row.Add(button);
  }

  void OnValueChanged(ProgressView, float value)
  {
    mTooltip.SetText(Dali::String(std::to_string(static_cast<int>(value)).c_str()) + "%");
    UpdateStatus();
  }

  void OnDeterminateChanged(ProgressView, bool determinate)
  {
    mTooltip.SetVisible(determinate);
    for(Label label : mDividerLabels)
    {
      label.SetVisible(determinate);
    }
    UpdateStatus();
  }

  void OnTrailEndPositionChanged(ProgressBar progressBar)
  {
    SyncTrail(progressBar);
  }

  void OnDividersChanged(ProgressBar progressBar)
  {
    SyncDividers(progressBar);
  }

  void SyncTrail(ProgressBar progressBar)
  {
    const Vector2 position = progressBar.GetTrailEndPosition();
    mTooltip.SetLayoutParams(AbsoluteLayoutParams::New()
                               .SetX(std::max(0.0f, position.x - 16.0f))
                               .SetY(std::max(0.0f, BAR_Y + position.y - 24.0f)));
  }

  void SyncDividers(ProgressBar progressBar)
  {
    for(Label label : mDividerLabels)
    {
      mOverlay.Remove(label);
    }
    mDividerLabels.clear();

    for(uint32_t index = 0u; index < progressBar.GetDividerCount(); ++index)
    {
      const Vector2 position = progressBar.GetDividerPosition(index);
      Label label = MakeLabel(Dali::String(std::to_string(index + 1u).c_str()), 12.0f);
      label.SetLayoutParams(AbsoluteLayoutParams::New()
                              .SetX(std::max(0.0f, position.x - 4.0f))
                              .SetY(BAR_Y + position.y + 8.0f));
      label.SetVisible(progressBar.IsDeterminate());
      mOverlay.Add(label);
      mDividerLabels.push_back(label);
    }
  }

  void UpdateStatus()
  {
    if(!mStatus || !mProgress)
    {
      return;
    }

    Dali::String text = "value " + Dali::String(std::to_string(static_cast<int>(mProgress.GetValue())).c_str()) + "%";
    text += mProgress.IsDeterminate() ? ", determinate" : ", indeterminate";
    text += mProgress.IsReversed() ? ", reversed" : ", normal";
    text += mRightToLeft ? ", RTL" : ", LTR";
    mStatus.SetText(text);
  }

private:
  AbsoluteLayout mOverlay;
  ProgressBar mProgress;
  Label mTooltip;
  Label mStatus;
  std::vector<Label> mDividerLabels;
  bool mRightToLeft{false};
};

REGISTER_MANUAL_TEST(TcProgressBarBasics)
