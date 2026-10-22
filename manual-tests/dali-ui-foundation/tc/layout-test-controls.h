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

#ifndef DALI_UI_MANUAL_LAYOUT_TEST_CONTROLS_H
#define DALI_UI_MANUAL_LAYOUT_TEST_CONTROLS_H

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <functional>
#include <initializer_list>
#include <utility>

namespace ManualTest
{
struct LayoutSizePreset
{
  const char* label;
  float       width;
  float       height;
};

inline Dali::Ui::FlexLayout MakeLayoutControlRow()
{
  using namespace Dali::Ui;
  FlexLayout row = FlexLayout::New();
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(60.0f);
  row.SetDirection(FlexDirection::ROW);
  row.SetAlignItems(FlexAlign::STRETCH);
  return row;
}

inline void AddLayoutControl(Dali::Ui::FlexLayout row, Dali::ConnectionTracker* tracker,
                             const char* text, std::function<void()> action)
{
  using namespace Dali;
  using namespace Dali::Ui;
  // Match the InteractiveView + centered Label controls used by the layout
  // direction TCs (52/64/71/88), keeping the label out of hit testing.
  InteractiveView button = InteractiveView::New();
  button.SetBackgroundColor(Color::BLACK);
  button.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(0.0f).SetFlexGrow(1.0f));
  button.SetMargin(Insets(4.0f, 4.0f, 0.0f, 0.0f));
  button.SetFocusable(true);
  Label label = Label::New(text);
  label.SetTextColor(Color::WHITE);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(MATCH_PARENT);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetFontSize(16.0f);
  label.SetProperty(Actor::Property::SENSITIVE, false);
  button.Add(label);
  button.ConnectClickedSignal(tracker, [action = std::move(action)](View, InputEvent) -> bool
  {
    action();
    return true;
  });
  row.Add(button);
}

// Keep controls outside the resized view. Only the host receives Stack weight:
// weighting the target itself would override its requested main-axis size.
inline void MountLayoutTest(Dali::Ui::View contentArea, Dali::Ui::View target, Dali::Ui::FlexLayout controls)
{
  using namespace Dali::Ui;
  StackLayout host = StackLayout::New();
  host.SetRequestedWidth(MATCH_PARENT);
  host.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
  host.Add(target);
  contentArea.Add(controls);
  contentArea.Add(host);
}

inline void MountResizableLayout(Dali::Ui::View contentArea, Dali::Ui::View target,
                                 Dali::ConnectionTracker*                tracker,
                                 std::initializer_list<LayoutSizePreset> presets = {
                                   {"Parent: Width 800", 800.0f, Dali::Ui::MATCH_PARENT},
                                   {"Parent: Height 600", Dali::Ui::MATCH_PARENT, 600.0f},
                                   {"Parent: 800x600", 800.0f, 600.0f},
                                   {"Parent: Full", Dali::Ui::MATCH_PARENT, Dali::Ui::MATCH_PARENT}})
{
  auto controls = MakeLayoutControlRow();
  for(const auto& preset : presets)
  {
    AddLayoutControl(controls, tracker, preset.label, [target, preset]() mutable
    {
      target.SetRequestedWidth(preset.width);
      target.SetRequestedHeight(preset.height);
    });
  }
  MountLayoutTest(contentArea, target, controls);
}
} // namespace ManualTest

#endif // DALI_UI_MANUAL_LAYOUT_TEST_CONTROLS_H
