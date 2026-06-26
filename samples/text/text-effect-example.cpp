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
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/math/math-utils.h>

using namespace Dali;
using namespace Dali::Ui;

/*
 * Text effect structure:
 *
 * Label glyph alpha
 *     ^ mask
 * TextEffect (AbsoluteLayout)
 *     |- base gradient visual
 *     `- animated shimmer band
 *
 * TextEffect owns the MaskEffect source view, the gradient fill, and the
 * shimmer animation. It is added as the Label's MaskEffect source and follows
 * the label size by constraint, without rebuilding the mask effect.
 * TextEffect is not added to the scene directly by the app.
 */
namespace
{
constexpr float STACK_SPACING            = 14.0f;
constexpr float STACK_PADDING            = 28.0f;
constexpr float EFFECT_WIDTH             = 640.0f;
constexpr float EFFECT_HEIGHT            = 180.0f;
constexpr float TEXT_FONT_SIZE           = 62.0f;
constexpr int   WINDOW_WIDTH             = 800;
constexpr int   WINDOW_HEIGHT            = 520;

constexpr const char* EFFECT_TEXT = "DALI UI FOUNDATION";

Label CreateTextLabel(const char* text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  return label;
}

class TextEffect : public AbsoluteLayout
{
private:
  struct Data
  {
    Data(View shimmer, Dali::Property::Index progress)
    : shimmerBand(shimmer),
      shimmerProgressIndex(progress)
    {
    }

    View                  shimmerBand;
    Dali::Property::Index shimmerProgressIndex;
    Animation             shimmerAnimation;
    bool                  shimmerRunning{false};
  };

public:
  TextEffect() = default;

  static TextEffect New()
  {
    TextEffect effect(AbsoluteLayout::New());
    effect.Initialize();
    return effect;
  }

  static TextEffect DownCast(BaseHandle handle)
  {
    AbsoluteLayout layout = AbsoluteLayout::DownCast(handle);
    return layout && layout.GetAttachment<Data>(GetDataId()) ? TextEffect(layout) : TextEffect();
  }

  void ApplyTo(Label label)
  {
    if(label && *this && GetData())
    {
      // Label owns this view as its MaskEffect source; TextEffect must not keep
      // a Label handle back to its owner.
      label.SetMaskEffect(*this);
      ResetShimmer(false);
    }
  }

  void StartShimmer()
  {
    Data* data = GetData();
    if(!data || !data->shimmerBand)
    {
      return;
    }

    data->shimmerRunning = true;

    ClearShimmerAnimation();
    ResetShimmer(true);

    data->shimmerAnimation = Animation::New(SHIMMER_DURATION_SECONDS);
    data->shimmerAnimation.AnimateTo(
      Dali::Property(data->shimmerBand, data->shimmerProgressIndex),
      1.0f,
      AlphaFunction::LINEAR);
    data->shimmerAnimation.SetLooping(true);
    data->shimmerAnimation.SetEndAction(Animation::DISCARD);
    data->shimmerAnimation.Play();
  }

  void StopShimmer()
  {
    Data* data = GetData();
    if(data)
    {
      data->shimmerRunning = false;
    }

    ClearShimmerAnimation();
    ResetShimmer(false);
  }

  bool IsShimmerRunning() const
  {
    const Data* data = GetData();
    return data ? data->shimmerRunning : false;
  }

private:
  explicit TextEffect(AbsoluteLayout layout)
  : AbsoluteLayout(layout)
  {
  }

  static constexpr float       SHIMMER_BAND_WIDTH           = 620.0f;
  static constexpr float       SHIMMER_DURATION_SECONDS     = 1.6f;
  static constexpr uint32_t    SHIMMER_INPUT_EFFECT_WIDTH   = 0u;
  static constexpr uint32_t    SHIMMER_INPUT_PROGRESS       = 1u;
  static constexpr uint32_t    SHIMMER_INPUT_COUNT          = 2u;
  static constexpr const char* SHIMMER_PROGRESS_PROPERTY    = "shimmerProgress";

  void Initialize()
  {
    SetRequestedWidth(MATCH_PARENT);
    SetRequestedHeight(MATCH_PARENT);
    SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

    ApplyParentSizeConstraints();

    GradientVisual baseGradient = GradientVisual::New();
    baseGradient.SetLinearGradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    baseGradient.SetStopNodes({
      {0.0f, UiColor(0.70f, 0.24f, 1.0f, 1.0f)},
      {0.45f, UiColor(0.45f, 0.28f, 1.0f, 1.0f)},
      {1.0f, UiColor(0.08f, 0.72f, 1.0f, 1.0f)},
    });

    // Compose the visible fill first, then mask the result once with the text label.
    AddVisual(baseGradient, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

    View                  shimmerBand = CreateShimmerBand();
    Dali::Property::Index progress    = shimmerBand.RegisterProperty(SHIMMER_PROGRESS_PROPERTY, 0.0f);
    Add(shimmerBand);

    Constraint shimmerPosition = Constraint::New<float>(shimmerBand, Actor::Property::POSITION_X, ShimmerPositionConstraint);
    // Source order must match ShimmerPositionConstraint input indices.
    shimmerPosition.AddSource(ParentSource(Actor::Property::SIZE_WIDTH));
    shimmerPosition.AddSource(LocalSource(progress));
    shimmerPosition.Apply();

    Constraint shimmerHeight = Constraint::New<float>(shimmerBand, Actor::Property::SIZE_HEIGHT, EqualToConstraint());
    shimmerHeight.AddSource(ParentSource(Actor::Property::SIZE_HEIGHT));
    shimmerHeight.Apply();

    SetAttachment(GetDataId(), Dali::MakeUnique<Data>(shimmerBand, progress));
  }

  View CreateShimmerBand()
  {
    View shimmerBand = View::New();
    shimmerBand.SetRequestedWidth(SHIMMER_BAND_WIDTH);
    shimmerBand.SetRequestedHeight(MATCH_PARENT);
    shimmerBand.SetOpacity(0.0f);
    shimmerBand.SetLayoutParams(
      AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(GetShimmerStartX(), 0.0f, SHIMMER_BAND_WIDTH, MATCH_PARENT)));

    // The band is moved by Actor::Property::POSITION_X, so keep the actor-space
    // origin and initial size explicit.
    shimmerBand.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    shimmerBand.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    shimmerBand.SetProperty(Actor::Property::SIZE_WIDTH, SHIMMER_BAND_WIDTH);
    shimmerBand.SetProperty(Actor::Property::POSITION_X, GetShimmerStartX());
    shimmerBand.SetProperty(Actor::Property::POSITION_Y, 0.0f);

    GradientVisual shimmerGradient = GradientVisual::New();
    shimmerGradient.SetLinearGradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    shimmerGradient.SetStopNodes({
      {0.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)},
      {0.28f, UiColor(1.0f, 1.0f, 1.0f, 0.10f)},
      {0.50f, UiColor(1.0f, 1.0f, 1.0f, 0.78f)},
      {0.72f, UiColor(1.0f, 1.0f, 1.0f, 0.10f)},
      {1.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)},
    });
    shimmerBand.AddVisual(shimmerGradient, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

    return shimmerBand;
  }

  void ClearShimmerAnimation()
  {
    Data* data = GetData();
    if(data && data->shimmerAnimation)
    {
      data->shimmerAnimation.Stop();
      data->shimmerAnimation.Clear();
    }
  }

  void ResetShimmer(bool visible)
  {
    Data* data = GetData();
    if(!data || !data->shimmerBand)
    {
      return;
    }

    data->shimmerBand.SetProperty(data->shimmerProgressIndex, 0.0f);
    data->shimmerBand.SetOpacity(visible ? 1.0f : 0.0f);
  }

  Data* GetData()
  {
    return GetAttachment<Data>(GetDataId());
  }

  const Data* GetData() const
  {
    return GetAttachment<Data>(GetDataId());
  }

  void ApplyParentSizeConstraints()
  {
    Constraint width = Constraint::New<float>(
      *this,
      Actor::Property::SIZE_WIDTH,
      EqualToConstraint());
    width.AddSource(ParentSource(Actor::Property::SIZE_WIDTH));
    width.Apply();

    Constraint height = Constraint::New<float>(
      *this,
      Actor::Property::SIZE_HEIGHT,
      EqualToConstraint());
    height.AddSource(ParentSource(Actor::Property::SIZE_HEIGHT));
    height.Apply();
  }

  static float GetShimmerStartX()
  {
    return -SHIMMER_BAND_WIDTH;
  }

  static void ShimmerPositionConstraint(float& current, const PropertyInputContainer& inputs)
  {
    DALI_ASSERT_DEBUG(inputs.Count() >= SHIMMER_INPUT_COUNT);

    const float effectWidth = inputs[SHIMMER_INPUT_EFFECT_WIDTH]->GetFloat();
    const float progress    = Dali::Clamp(inputs[SHIMMER_INPUT_PROGRESS]->GetFloat(), 0.0f, 1.0f);

    current = GetShimmerStartX() + progress * (effectWidth + SHIMMER_BAND_WIDTH * 2.0f);
  }

  static AttachmentId GetDataId()
  {
    static AttachmentId id = AttachmentId::Alloc();
    return id;
  }
};

Label CreateEffectTextLabel(float width, float height)
{
  Label label = Label::New(EFFECT_TEXT);
  label.SetFontFamily("SamsungOneUI_700");
  label.SetFontSize(TEXT_FONT_SIZE);
  label.SetTextColor(UiColor(0xFFFFFF));
  label.SetMultiLine(true);
  label.SetRequestedWidth(width);
  label.SetRequestedHeight(height);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  return label;
}
} // namespace

class TextEffectController : public ConnectionTracker
{
public:
  explicit TextEffectController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextEffectController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    auto positionSize = window.GetPositionSize();
    window.SetPositionSize(Dali::PositionSize(positionSize.x, positionSize.y, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    root.Add(CreateTextLabel("Gradient text using MaskEffect", 16.0f, UiColor(0x4B5563)));
    mTextEffect = TextEffect::New();
    mEffectLabel = CreateEffectTextLabel(WRAP_CONTENT, WRAP_CONTENT);
    mTextEffect.ApplyTo(mEffectLabel);
    root.Add(mEffectLabel);
    root.Add(CreateTextLabel(
      "1: Start shimmer   2: Stop shimmer   3: Fixed   4: Match   5: Wrap   ESC/BACK: Quit",
      16.0f,
      UiColor(0x6B7280)));

    StartShimmer();

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextEffectController::OnKeyEvent);
  }

  void SetEffectSize(float width, float height)
  {
    if(mEffectLabel)
    {
      mEffectLabel.SetRequestedWidth(width);
      mEffectLabel.SetRequestedHeight(height);
    }
  }

  void StartShimmer()
  {
    mTextEffect.StartShimmer();
  }

  void StopShimmer()
  {
    mTextEffect.StopShimmer();
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
      StartShimmer();
    }
    else if(event.GetKeyName() == "2")
    {
      StopShimmer();
    }
    else if(event.GetKeyName() == "3")
    {
      SetEffectSize(EFFECT_WIDTH, EFFECT_HEIGHT);
    }
    else if(event.GetKeyName() == "4")
    {
      SetEffectSize(MATCH_PARENT, WRAP_CONTENT);
    }
    else if(event.GetKeyName() == "5")
    {
      SetEffectSize(WRAP_CONTENT, WRAP_CONTENT);
    }
  }

private:
  Application& mApplication;
  Label        mEffectLabel;
  TextEffect   mTextEffect;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextEffectController controller(application);
  application.MainLoop();

  return 0;
}
