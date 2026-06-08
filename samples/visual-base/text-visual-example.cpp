/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <random>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;

constexpr int16_t VIEW_PADDING = 40;
constexpr int16_t VIEW_MARGIN = 40;

const char* SAMPLE_TEXTS[] = {
  "Hello World",
  "Dali UI",
  "Text Visual",
  "Samsung",
  "Typography",
  "Chain Setter",
  "Mutable Property",
  "Cutout Text",
};
constexpr uint32_t SAMPLE_TEXT_COUNT = sizeof(SAMPLE_TEXTS) / sizeof(SAMPLE_TEXTS[0]);

int GetRandomInteger(int from, int to)
{
  static std::mt19937 rand;

  if(from >= to)
  {
    return from;
  }

  return rand() % (to - from) + from;
}

float GetRandomFloat()
{
  constexpr int MAX_INTEGER = 10000;
  int rand = GetRandomInteger(0, MAX_INTEGER);
  return static_cast<float>(rand) / static_cast<float>(MAX_INTEGER);
}

} // namespace

class TextVisualController : public ConnectionTracker
{
public:
  explicit TextVisualController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextVisualController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    Label titleLabel = Label::New("TextVisual Example");
    titleLabel.SetRequestedWidth(WRAP_CONTENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);
    root.Add(titleLabel);

    mVisualCounter = Label::New("");
    mVisualCounter.SetRequestedWidth(WRAP_CONTENT);
    mVisualCounter.SetRequestedHeight(WRAP_CONTENT);
    root.Add(mVisualCounter);

    Label contentLabel = Label::New("Contents");
    contentLabel.SetBackgroundColor(UiColor(0x003070).WithAlpha(0.5f));
    contentLabel.SetFontFamily("SamsungOneUI_700");
    contentLabel.SetTextColor(UiColor(0x000000).WithAlpha(1.0f));
    contentLabel.SetTextFit(Text::FitRange(20, 1000, 10));
    contentLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    contentLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    contentLabel.SetRequestedWidth(MATCH_PARENT);
    contentLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    contentLabel.SetPadding(Extents(VIEW_PADDING, VIEW_PADDING, VIEW_PADDING, VIEW_PADDING));
    contentLabel.SetMargin(Extents(VIEW_MARGIN, VIEW_MARGIN, VIEW_MARGIN, VIEW_MARGIN));
    contentLabel.SetCornerRadius(0.25f);
    contentLabel.SetCornerSquareness(0.6f);
    contentLabel.SetCornerRadiusPolicyRelative();
    contentLabel.SetBorderlineWidth(2_spx);
    contentLabel.SetBorderlineColor(UiColor(0x3F3F3F).WithAlpha(0.5f));
    contentLabel.SetBorderlineOffset(-0.98f);
    mView = contentLabel;
    mView.AddVisuals( // Custom Shadow
      Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND, {
        ColorVisual::New()
          .SetName("CustomShadow1")
          .SetOffsetX(10_spx)
          .SetOffsetY(-10_spx)
          .SetWidth(1.01f)
          .SetHeight(1.0f)
          .SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL)
          .SetExtraWidth(50_spx)
          .SetExtraHeight(-20_spx)
          .SetOrigin(Align::TOP_CENTER)
          .SetPivot(Align::TOP_CENTER)
          .SetBlurRadius(12_spx)
          .SetCornerRadius(12_spx)
          .SetColor(UiColor(0x3F0F0F).WithAlpha(0.2f))
          .SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS),
        ColorVisual::New()
          .SetName("CustomShadow2")
          .SetOffsetX(-10_spx)
          .SetOffsetY(-20_spx)
          .SetWidth(1.01f)
          .SetHeight(1.01f)
          .SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL)
          .SetExtraWidth(0_spx)
          .SetExtraHeight(0_spx)
          .SetOrigin(Align::TOP_BEGIN)
          .SetPivot(Align::TOP_BEGIN)
          .SetBlurRadius(15_spx)
          .SetCornerRadius(15_spx)
          .SetColor(UiColor(0x7F7FCF).WithAlpha(0.3f))
          .SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS),
        ColorVisual::New()
          .SetName("CustomShadow3")
          .SetOffsetX(10_spx)
          .SetOffsetY(20_spx)
          .SetWidth(1.0f)
          .SetHeight(1.01f)
          .SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL)
          .SetExtraWidth(10_spx)
          .SetExtraHeight(-10_spx)
          .SetOrigin(Align::TOP_END)
          .SetPivot(Align::TOP_END)
          .SetBlurRadius(10_spx)
          .SetCornerRadius(10_spx)
          .SetColor(UiColor(0x0F040F).WithAlpha(0.3f))
          .SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS),
      });
    mView.AddVisuals( // Custom InnerShadow
      Visual::ContainerRangeType::BETWEEN_DECORATION_AND_FOREGROUND_EFFECT, {
        ColorVisual::New()
          .SetName("CustomInnerShadow1")
          .SetOffsetX(10_spx)
          .SetOffsetY(20_spx)
          .SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL)
          .SetOrigin(Align::CENTER)
          .SetPivot(Align::CENTER)
          .SetCornerRadius(0.25f)
          .SetCornerSquareness(0.6f)
          .SetCornerRadiusPolicyRelative()
          .SetBlurRadius(40_spx)
          .SetBorderlineWidth(30_spx)
          .SetBorderlineColor(UiColor(0xFFFFFF).WithAlpha(0.6f))
          .SetBorderlineOffset(1.0f)
          .SetColor(UiColor(0x000000).WithAlpha(0.0f))
          .SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS),
        ColorVisual::New()
          .SetName("CustomInnerShadow2")
          .SetOffsetX(-10_spx)
          .SetOffsetY(-20_spx)
          .SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL)
          .SetOrigin(Align::CENTER)
          .SetPivot(Align::CENTER)
          .SetCornerRadius(0.25f)
          .SetCornerSquareness(0.6f)
          .SetCornerRadiusPolicyRelative()
          .SetBlurRadius(40_spx)
          .SetBorderlineWidth(30_spx)
          .SetBorderlineColor(UiColor(0x000000).WithAlpha(0.6f))
          .SetBorderlineOffset(1.0f)
          .SetColor(UiColor(0x000000).WithAlpha(0.0f))
          .SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS),
      });
    root.Add(mView);

    StackLayout buttonRow = StackLayout::New(StackOrientation::HORIZONTAL);
    buttonRow.SetSpacing(STACK_SPACING);
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(WRAP_CONTENT);
    buttonRow.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    AbsoluteLayout addButton = AbsoluteLayout::New();
    addButton.SetBackgroundColor(UiColor(0xFF0000));
    addButton.SetRequestedHeight(50_spx);
    addButton.SetLayoutParams(StackLayoutParams::New().SetWeight(0.25f));
    InteractiveTrait addInteractive = addButton.AsInteractive();
    addInteractive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      PushTextVisual();
      return true;
    });

    Label addLabel = Label::New("Add");
    addLabel.SetLayoutParams(AbsoluteLayoutParams::New()
                               .SetX(0.5f)
                               .SetY(0.5f)
                               .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    addLabel.SetRequestedWidth(WRAP_CONTENT);
    addLabel.SetRequestedHeight(WRAP_CONTENT);
    addLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    addLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    addButton.Add(addLabel);
    buttonRow.Add(addButton);

    AbsoluteLayout removeButton = AbsoluteLayout::New();
    removeButton.SetBackgroundColor(UiColor(0x00FF00));
    removeButton.SetRequestedHeight(50_spx);
    removeButton.SetLayoutParams(StackLayoutParams::New().SetWeight(0.25f));
    InteractiveTrait removeInteractive = removeButton.AsInteractive();
    removeInteractive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      PopVisual();
      return true;
    });

    Label removeLabel = Label::New("Remove");
    removeLabel.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX(0.5f)
                                  .SetY(0.5f)
                                  .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    removeLabel.SetRequestedWidth(WRAP_CONTENT);
    removeLabel.SetRequestedHeight(WRAP_CONTENT);
    removeLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    removeLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    removeButton.Add(removeLabel);
    buttonRow.Add(removeButton);

    root.Add(buttonRow);
    window.Add(root);

    // Add initial text visuals
    PushTextVisual();

    UpdateVisualCount();
    window.KeyEventSignal().Connect(this, &TextVisualController::OnKeyEvent);
  }

  void PushTextVisual()
  {
    const char* text = SAMPLE_TEXTS[GetRandomInteger(0, SAMPLE_TEXT_COUNT)];
    float fontSize = GetRandomFloat() * 30.0f + 15.0f;

    TextVisual visual = TextVisual::New()
      .SetText(text)
      .SetFontSize(fontSize)
      .SetTextColor(UiColor(GetRandomFloat(), GetRandomFloat(), GetRandomFloat(), 1.0f))
      .SetHorizontalAlignment(Text::Alignment::CENTER)
      .SetVerticalAlignment(Text::Alignment::CENTER)
      .SetWidth(GetRandomFloat() * 0.4f + 0.4f)
      .SetHeight(GetRandomFloat() * 0.2f + 0.1f)
      .SetOffsetX(GetRandomFloat() * 0.2f - 0.1f)
      .SetOffsetY(GetRandomFloat() * 0.2f - 0.1f)
      .SetProportionFlags(Visual::Transform::ProportionFlags::ALL);

    mView.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    UpdateVisualCount();
  }

  void PopVisual()
  {
    uint32_t visualCount = mView.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    if(visualCount > 0u)
    {
      auto visual = mView.GetVisualAt(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, visualCount - 1u);
      mView.RemoveVisual(visual);
      UpdateVisualCount();
    }
  }

  void UpdateVisualCount()
  {
    std::string numberOfVisuals = std::to_string(mView.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT));
    mVisualCounter.SetText((std::string("Visuals Count : #") + numberOfVisuals).c_str());
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }
    if(event.GetKeyName() == "1")
    {
      PushTextVisual();
    }
    else if(event.GetKeyName() == "2")
    {
      PopVisual();
    }
  }

private:
  Application& mApplication;
  View mView;
  Label mVisualCounter;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextVisualController controller(application);
  application.MainLoop();

  return 0;
}
