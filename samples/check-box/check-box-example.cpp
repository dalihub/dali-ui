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

// Interactive sample for Ui::CheckBox.
//
// The CheckBox is a binary (checked/unchecked) selectable control whose glyph is a
// single Lottie animation. A user tap toggles it and (in AUTO / ENABLED animation
// modes) plays the check/uncheck animation; a programmatic SetSelected snaps instantly.
// The control does not define an outer requested size, so every CheckBox below is sized by the
// application (SetRequestedWidth/Height); the default style draws a fixed 36 glyph inside 8 padding.
//
// Demonstrated: box-only, labelled, pre-checked, toggle-by-click disabled, and the
// three SelectionAnimationMode values. Tapping any checkbox updates the status line.
//
// Press Escape or Back to quit.

#include <dali-ui-components/public-api/check-box.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Large enough to contain the default 36 box plus the default 8+8 padding (>= 52).
constexpr float CHECKBOX_HEIGHT = 56.0f;

// Capture-less icon generator for CheckBoxStyle::SetIconGenerator(). The app supplies the
// glyph url together with the EXPLICIT integer frame ranges played on select / deselect and the
// inner-fill recolour key path. Here we reuse the shipped asset (via the default style's icon)
// and pass the same [0,19] / [20,38] segments and key path ("check_box " has a trailing space,
// matching the asset layer); a real app can swap in its own asset. Must be a free function
// (IconGenerator = Ui::Callback<SelectableImageInterface()>).
SelectableImageInterface MakeSampleIcon()
{
  // Recover the shipped checkbox.json url from the default icon's drawing view.
  SelectableImageInterface defaultIcon = CheckBoxStyle::Default().CreateIcon();
  Dali::String             url         = LottieAnimationView::DownCast(defaultIcon.GetView()).GetResourceUrl();
  return SelectableLottieAnimationView::New(
    SelectableLottieImage(url,
                          SelectableLottieImage::FrameRange(0, 19),
                          SelectableLottieImage::FrameRange(20, 38),
                          "check_box .inner_fill.color"));
}

Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}
} // namespace

class CheckBoxExample : public ConnectionTracker
{
public:
  explicit CheckBoxExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &CheckBoxExample::Create);
  }

  ~CheckBoxExample() = default;

  void Create(Application application)
  {
    // CheckBox reads its style from the applied Components UiConfig.
    Components::UiConfig::New().Apply();

    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Insets(24.0f, 24.0f, 24.0f, 24.0f));
    mRoot = root;

    root.Add(MakeLabel("CheckBox Sample", 24.0f, 0x202124u));

    // Status line — created up front so a pre-checked SetSelected can safely update it
    // (the SelectionChangedSignal fires during Create()).
    mStatus = MakeLabel("Tap a checkbox…", 14.0f, 0x5F6368u);

    // 1. Box-only (default AUTO animation): app sizes it 48x48.
    {
      CheckBox cb = CheckBox::New();
      cb.SetRequestedWidth(CHECKBOX_HEIGHT);
      cb.SetRequestedHeight(CHECKBOX_HEIGHT);
      cb.SelectionChangedSignal().Connect(this, &CheckBoxExample::OnSelectionChanged);
      root.Add(cb);
    }

    // 2. Labelled.
    MakeCheckBox("I agree to the terms");

    // 3. Pre-checked (programmatic set snaps, no animation).
    {
      CheckBox cb = MakeCheckBox("Subscribe (pre-checked)");
      cb.SetSelected(true);
      // (already added by MakeCheckBox)
    }

    // 4. Toggle-by-click disabled (a tap is ignored; SetSelected still works).
    {
      CheckBox cb = MakeCheckBox("Read-only (click ignored)");
      cb.SetToggleByClickEnabled(false);
    }

    // 5. Always animate.
    {
      CheckBox cb = MakeCheckBox("Always animate");
      cb.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
    }

    // 6. Never animate (instant snap on every change).
    {
      CheckBox cb = MakeCheckBox("Never animate");
      cb.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);
    }

    // 7. Custom icon generator: the app supplies a factory that builds the selectable image
    // (glyph url + EXPLICIT integer frame ranges played on select / deselect). See
    // MakeSampleIcon() above; a real app can swap in its own asset.
    {
      CheckBoxStyle style = CheckBoxStyle::Default()
                              .Configure()
                              .SetIconGenerator(CheckBoxStyle::IconGenerator::New(&MakeSampleIcon))
                              .Build();

      CheckBox cb = CheckBox::New("Custom Lottie frame ranges", style);
      cb.SetRequestedWidth(WRAP_CONTENT);
      cb.SetRequestedHeight(CHECKBOX_HEIGHT);
      cb.SelectionChangedSignal().Connect(this, &CheckBoxExample::OnSelectionChanged);
      root.Add(cb);
    }

    root.Add(mStatus);

    window.Add(root);
  }

private:
  // Creates a labelled CheckBox sized by the app, wires the signal, and adds it to root.
  CheckBox MakeCheckBox(const Dali::String& text)
  {
    CheckBox cb = CheckBox::New(text);
    cb.SetRequestedWidth(WRAP_CONTENT);
    cb.SetRequestedHeight(CHECKBOX_HEIGHT);
    cb.SelectionChangedSignal().Connect(this, &CheckBoxExample::OnSelectionChanged);
    mRoot.Add(cb);
    return cb;
  }

  void OnSelectionChanged(View view, bool selected, InputEvent /*event*/)
  {
    if(!mStatus)
    {
      return; // status line not built yet
    }
    CheckBox     cb   = CheckBox::DownCast(view);
    Dali::String name = (cb && !cb.GetText().Empty()) ? cb.GetText() : Dali::String("(box-only)");
    mStatus.SetText(name + (selected ? " : checked" : " : unchecked"));
  }

private:
  Application& mApplication;
  StackLayout  mRoot;
  Label        mStatus;
};

int main(int argc, char** argv)
{
  Application     application = Application::New(&argc, &argv);
  CheckBoxExample example(application);
  application.MainLoop();
  return 0;
}
