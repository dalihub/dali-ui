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

constexpr std::string_view IMAGE_PATH[] = {
  RESOURCES_DIR "gallery-large-1.jpg",
  RESOURCES_DIR "gallery-large-3.jpg",
  RESOURCES_DIR "gallery-medium-3.jpg",
  RESOURCES_DIR "gallery-medium-49.jpg",
  RESOURCES_DIR "animatedLoading.gif",
  RESOURCES_DIR "dali-log-anim.gif",
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

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetSpacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({
          Label::New("AnimatedImageVisual Example")
            .SetRequestedWidth(WRAP_CONTENT)
            .SetRequestedHeight(WRAP_CONTENT),
          Label::New("")
            .SetRequestedWidth(WRAP_CONTENT)
            .SetRequestedHeight(WRAP_CONTENT)
            .As(mVisualState),
          Label::New("Contents")
            .SetBackgroundColor(UiColor(0x003070).WithAlpha(0.5f))
            .SetFontFamily("SamsungOneUI_700")
            .SetTextColor(UiColor(0x000000).WithAlpha(1.0f))
            .SetTextFit(Text::FitRange(20, 1000, 10))
            .SetHorizontalTextAlignment(Text::Alignment::CENTER)
            .SetVerticalTextAlignment(Text::Alignment::CENTER)
            .SetRequestedWidth(MATCH_PARENT)
            .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
            .SetPadding(Extents(VIEW_PADDING, VIEW_PADDING, VIEW_PADDING, VIEW_PADDING))
            .SetMargin(Extents(VIEW_MARGIN, VIEW_MARGIN, VIEW_MARGIN, VIEW_MARGIN))
            .SetCornerRadius(0.25f)
            .SetCornerSquareness(0.6f)
            .SetCornerRadiusPolicyRelative()
            .SetBorderlineWidth(2_spx)
            .SetBorderlineColor(UiColor(0x3F3F3F).WithAlpha(0.5f))
            .SetBorderlineOffset(-0.98f)
            .Visuals( // Custom Shadow
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
              })
            .Visuals( // Custom InnerShadow
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
              })
            .Visuals( // AnimatedImageVisual
              Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, {
                AnimatedImageVisual::New()
                  .SetResourceUrl(RESOURCES_DIR "dali-logo-anim.gif")
                  .SetWidth(0.5f)
                  .SetCornerRadius(0.25f, 0.0f, 0.0f, 0.25f)
                  .SetCornerRadiusPolicyRelative()
                  .SetCornerSquareness(0.6f)
                  .Play()
                  .As(mLeftVisual),
                AnimatedImageVisual::New()
                  .SetResourceUrlList({
                    RESOURCES_DIR "gallery-large-1.jpg",
                    RESOURCES_DIR "gallery-large-3.jpg",
                    RESOURCES_DIR "gallery-medium-3.jpg",
                    RESOURCES_DIR "gallery-medium-49.jpg",
                  })
                  .SetOffsetX(0.5f)
                  .SetWidth(0.5f)
                  .SetCornerRadius(0.0f, 0.25f, 0.25f, 0.0f)
                  .SetCornerRadiusPolicyRelative()
                  .SetCornerSquareness(0.6f)
                  .SetFrameDelay(1000.0f)
                  .Play()
                  .As(mRightVisual),
              })
            .As(mView),
          StackLayout::New(StackOrientation::HORIZONTAL)
            .SetSpacing(STACK_SPACING)
            .SetRequestedWidth(MATCH_PARENT)
            .SetRequestedHeight(WRAP_CONTENT)
            .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
            .Children({
              AbsoluteLayout::New()
                .SetBackgroundColor(UiColor(0xFF0000))
                .SetRequestedHeight(50_spx)
                .SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f))
                .AsInteractive([this](InteractiveTrait& trait) {
                  trait.ClickedSignal().Connect(this, [this](View, const InputEvent&) -> bool
                  {
                    ChangeLeftVisual();
                    return true;
                  });
                })
                .Children({
                  Label::New("Change Left Visual State")
                    .SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetX(0.5f)
                      .SetY(0.5f)
                      .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL))
                    .SetRequestedWidth(WRAP_CONTENT)
                    .SetRequestedHeight(WRAP_CONTENT)
                    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                    .SetVerticalTextAlignment(Text::Alignment::CENTER),
                }),
              AbsoluteLayout::New()
                .SetBackgroundColor(UiColor(0x00FF00))
                .SetRequestedHeight(50_spx)
                .SetLayoutParams(StackLayoutParams::New().SetWeight(0.5f))
                .AsInteractive([this](InteractiveTrait& trait) {
                  trait.ClickedSignal().Connect(this, [this](View, const InputEvent&) -> bool
                  {
                    ChangeRightVisual();
                    return true;
                  });
                })
                .Children({
                  Label::New("Change Right Visual State")
                    .SetLayoutParams(AbsoluteLayoutParams::New()
                      .SetX(0.5f)
                      .SetY(0.5f)
                      .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL))
                    .SetRequestedWidth(WRAP_CONTENT)
                    .SetRequestedHeight(WRAP_CONTENT)
                    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
                    .SetVerticalTextAlignment(Text::Alignment::CENTER),
                }),
            }),
        }));
  
    UpdateVisualState();
    window.KeyEventSignal().Connect(this, &VisualBaseController::OnKeyEvent);
  }

  void ChangeLeftVisual()
  {
    if(mLeftVisual.GetPlayState() == Ui::AnimatedImage::PlayState::PLAYING)
    {
      mLeftVisual.Stop();
    }
    else
    {
      mLeftVisual.Play();
    }
    UpdateVisualState();
  }
  void ChangeRightVisual()
  {
    if(mRightVisual.GetPlayState() == Ui::AnimatedImage::PlayState::PLAYING)
    {
      mRightVisual.Stop();
    }
    else
    {
      mRightVisual.Play();
    }
    UpdateVisualState();
  }

  void UpdateVisualState()
  {
    // TODO : Need to call this function at finished signal of visual :(
    std::string leftVisualState  = std::to_string(static_cast<int32_t>(mLeftVisual.GetPlayState()));
    std::string rightVisualState = std::to_string(static_cast<int32_t>(mRightVisual.GetPlayState()));

    mVisualState.SetText((std::string("Left : ") + leftVisualState + " Right : " + rightVisualState).c_str());
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
      ChangeLeftVisual();
    }
    else if(event.GetKeyName() == "2")
    {
      ChangeRightVisual();
    }
  }

private:
  Application& mApplication;
  View  mView;
  Label mVisualState;

  AnimatedImageVisual mLeftVisual;
  AnimatedImageVisual mRightVisual;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  VisualBaseController controller(application);
  application.MainLoop();

  return 0;
}
