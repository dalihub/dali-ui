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

int GetRandomInteger(int from, int to)
{
  static std::mt19937 rand;

  if(from >= to)
  {
    // Error!
    return from;
  }

  return rand()%(to-from) + from;
}

float GetRandomFloat()
{
  constexpr int MAX_INTEGER = 10000;
  int rand = GetRandomInteger(0, MAX_INTEGER);
  return static_cast<float>(rand) / static_cast<float>(MAX_INTEGER);
}

ColorVisual CreateCustomShadow1()
{
  ColorVisual visual = ColorVisual::New();
  visual.SetName("CustomShadow1");
  visual.SetOffsetX(10_spx);
  visual.SetOffsetY(-10_spx);
  visual.SetWidth(1.01f);
  visual.SetHeight(1.0f);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetExtraWidth(50_spx);
  visual.SetExtraHeight(-20_spx);
  visual.SetOrigin(Align::TOP_CENTER);
  visual.SetPivot(Align::TOP_CENTER);
  visual.SetBlurRadius(12_spx);
  visual.SetCornerRadius(12_spx);
  visual.SetColor(UiColor(0x3F0F0F).WithAlpha(0.2f));
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  return visual;
}

ColorVisual CreateCustomShadow2()
{
  ColorVisual visual = ColorVisual::New();
  visual.SetName("CustomShadow2");
  visual.SetOffsetX(-10_spx);
  visual.SetOffsetY(-20_spx);
  visual.SetWidth(1.01f);
  visual.SetHeight(1.01f);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetExtraWidth(0_spx);
  visual.SetExtraHeight(0_spx);
  visual.SetOrigin(Align::TOP_BEGIN);
  visual.SetPivot(Align::TOP_BEGIN);
  visual.SetBlurRadius(15_spx);
  visual.SetCornerRadius(15_spx);
  visual.SetColor(UiColor(0x7F7FCF).WithAlpha(0.3f));
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  return visual;
}

ColorVisual CreateCustomShadow3()
{
  ColorVisual visual = ColorVisual::New();
  visual.SetName("CustomShadow3");
  visual.SetOffsetX(10_spx);
  visual.SetOffsetY(20_spx);
  visual.SetWidth(1.0f);
  visual.SetHeight(1.01f);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetExtraWidth(10_spx);
  visual.SetExtraHeight(-10_spx);
  visual.SetOrigin(Align::TOP_END);
  visual.SetPivot(Align::TOP_END);
  visual.SetBlurRadius(10_spx);
  visual.SetCornerRadius(10_spx);
  visual.SetColor(UiColor(0x0F040F).WithAlpha(0.3f));
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  return visual;
}

ColorVisual CreateCustomInnerShadow1()
{
  ColorVisual visual = ColorVisual::New();
  visual.SetName("CustomInnerShadow1");
  visual.SetOffsetX(10_spx);
  visual.SetOffsetY(20_spx);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetOrigin(Align::CENTER);
  visual.SetPivot(Align::CENTER);
  visual.SetCornerRadius(0.25f);
  visual.SetCornerSquareness(0.6f);
  visual.SetCornerRadiusPolicyRelative();
  visual.SetBlurRadius(40_spx);
  visual.SetBorderlineWidth(30_spx);
  visual.SetBorderlineColor(UiColor(0xFFFFFF).WithAlpha(0.6f));
  visual.SetBorderlineOffset(1.0f);
  visual.SetColor(UiColor(0x000000).WithAlpha(0.0f));
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS);
  return visual;
}

ColorVisual CreateCustomInnerShadow2()
{
  ColorVisual visual = ColorVisual::New();
  visual.SetName("CustomInnerShadow2");
  visual.SetOffsetX(-10_spx);
  visual.SetOffsetY(-20_spx);
  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  visual.SetOrigin(Align::CENTER);
  visual.SetPivot(Align::CENTER);
  visual.SetCornerRadius(0.25f);
  visual.SetCornerSquareness(0.6f);
  visual.SetCornerRadiusPolicyRelative();
  visual.SetBlurRadius(40_spx);
  visual.SetBorderlineWidth(30_spx);
  visual.SetBorderlineColor(UiColor(0x000000).WithAlpha(0.6f));
  visual.SetBorderlineOffset(1.0f);
  visual.SetColor(UiColor(0x000000).WithAlpha(0.0f));
  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS);
  return visual;
}

void AddDecorativeVisuals(View view)
{
  view.AddVisuals(
    Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND,
    {
      CreateCustomShadow1(),
      CreateCustomShadow2(),
      CreateCustomShadow3(),
    });
  view.AddVisuals(
    Visual::ContainerRangeType::BETWEEN_DECORATION_AND_FOREGROUND_EFFECT,
    {
      CreateCustomInnerShadow1(),
      CreateCustomInnerShadow2(),
    });
}

constexpr std::string_view IMAGE_PATH[] = {
  RESOURCES_DIR "gallery-large-1.jpg",
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "animatedLoading.gif",
  RESOURCES_DIR "dali-logo-anim.gif",
  RESOURCES_DIR "dog-anim.webp",
  RESOURCES_DIR "exif-rotated.jpg",
};
constexpr int IMAGE_PATH_COUNT= sizeof(IMAGE_PATH) / sizeof(IMAGE_PATH[0]);
} // namespace

class VisualBaseController : public ConnectionTracker
{
public:
  explicit VisualBaseController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &VisualBaseController::OnInit);
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
    root.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));

    Label titleLabel = Label::New("ImageVisual Example");
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
    contentLabel.SetTextFit(Text::Fit::Range(20, 1000, 10));
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
    AddDecorativeVisuals(mView);
    root.Add(mView);

    StackLayout buttonRow = StackLayout::New(StackOrientation::HORIZONTAL);
    buttonRow.SetSpacing(STACK_SPACING);
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(WRAP_CONTENT);
    buttonRow.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));

    AbsoluteLayout addButton = AbsoluteLayout::New();
    addButton.SetBackgroundColor(UiColor(0xFF0000));
    addButton.SetRequestedHeight(50_spx);
    addButton.SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f));
    InteractiveTrait addInteractive = addButton.AsInteractive();
    addInteractive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      PushVisual();
      return true;
    });

    Label addLabel = Label::New("Add Visual");
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
    removeButton.SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f));
    InteractiveTrait removeInteractive = removeButton.AsInteractive();
    removeInteractive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      PopVisual();
      return true;
    });

    Label removeLabel = Label::New("Remove Visual");
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
  
    // Add initial visuals
    for(int i=0; i<3; i++)
    {
      PushVisual();
    }

    UpdateVisualCount();
    window.KeyEventSignal().Connect(this, &VisualBaseController::OnKeyEvent);
  }

  void PushVisual()
  {
    int imageIndex = GetRandomInteger(0, IMAGE_PATH_COUNT);
    ImageVisual visual = ImageVisual::New();
    visual.SetColor(UiColor(1.0f, 1.0f, 1.0f, GetRandomFloat() * 0.5f + 0.5f));
    visual.SetOffsetX(GetRandomFloat() * 0.2f);
    visual.SetOffsetY(GetRandomFloat() * 0.2f);
    visual.SetWidth(GetRandomFloat() * 0.4f + 0.4f);
    visual.SetHeight(GetRandomFloat() * 0.4f + 0.4f);
    visual.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);
    visual.SetCornerRadius(GetRandomFloat() * 0.5f);
    visual.SetCornerRadiusPolicyRelative();
    visual.SetResourceUrl(Dali::StringView(IMAGE_PATH[imageIndex].data(), static_cast<uint32_t>(IMAGE_PATH[imageIndex].size())));

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
      PushVisual();
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
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();

  VisualBaseController controller(application);
  application.MainLoop();

  return 0;
}
