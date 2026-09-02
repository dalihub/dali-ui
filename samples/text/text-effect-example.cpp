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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/math/math-utils.h>

using namespace Dali;
using namespace Dali::Ui;

/*
 * This sample compares three ways to create a text shimmer effect.
 *
 * 1. MaskEffect shimmer
 *
 *    Scene / object structure:
 *
 *    Label                         : visible text and glyph-alpha mask owner
 *    `- MaskEffect source
 *       `- MaskEffectShimmerSource : AbsoluteLayout source view
 *          |- base GradientVisual  : base gradient fill
 *          `- shimmerBand View     : real child view animated horizontally
 *             `- GradientVisual    : transparent-white-transparent shimmer band
 *
 *    Cost model:
 *      - 3 View-like objects are involved:
 *          Label + MaskEffectShimmerSource + shimmerBand View
 *      - 2 GradientVisuals are used:
 *          base gradient + shimmer gradient
 *      - The shimmer band is a real View that moves.
 *      - The final result is clipped by the Label glyph alpha through MaskEffect.
 *
 * 2. TextGradient + TextGradientOverlay shimmer
 *
 *    Scene / object structure:
 *
 *    Label only
 *      |- TextGradient             : base glyph fill, evaluated in the text shader
 *      `- TextGradientOverlay      : transparent-white-transparent overlay gradient
 *         `- animated startOffset  : moves the sampled highlight in the shader
 *
 *    Cost model:
 *      - 1 View-like object:
 *          Label
 *      - No extra shimmer View.
 *      - No GradientVisual is used by the application.
 *      - The visible "band" is the bright region of the overlay gradient lookup.
 *      - Motion comes from animating uTextGradientOverlayStartOffset.
 *
 * 3. Minimal TextGradientOverlay shimmer
 *
 *    Scene / object structure:
 *
 *    Label only
 *      |- TextColor                : plain gray base glyph fill
 *      `- TextGradientOverlay      : SCREEN highlight overlay
 *         `- animated startOffset  : moves the highlight in the shader
 *
 *    Cost model:
 *      - 1 View-like object:
 *          Label
 *      - No base TextGradient.
 *      - No MaskEffect source view.
 *      - No shimmer band View.
 *      - No GradientVisual is used by the application.
 *
 * In short:
 *
 *   MaskEffect version:
 *     Label + MaskEffectShimmerSource + shimmerBand View, with 2 GradientVisuals.
 *
 *   TextGradientOverlay versions:
 *     Label only. The shimmer is shader-side gradient sampling driven by
 *     TextGradientOverlayStartOffset animation.
 */
namespace
{
// -----------------------------------------------------------------------------
// Common constants
// -----------------------------------------------------------------------------

constexpr float STACK_SPACING                    = 14.0f;
constexpr float STACK_PADDING                    = 28.0f;
constexpr float EFFECT_WIDTH                     = 640.0f;
constexpr float EFFECT_HEIGHT                    = 180.0f;
constexpr float TEXT_FONT_SIZE                   = 62.0f;
constexpr float SHIMMER_BAND_WIDTH               = 920.0f;
constexpr float SHIMMER_DURATION_SECONDS         = 1.4f;
constexpr float OVERLAY_SHIMMER_DURATION_SECONDS = 1.4f;
constexpr float OVERLAY_SHIMMER_START_OFFSET     = 1.0f;
constexpr float OVERLAY_SHIMMER_END_OFFSET       = -1.0f;
constexpr int   WINDOW_WIDTH                     = 800;
constexpr int   WINDOW_HEIGHT                    = 720;
constexpr uint32_t BACKGROUND_COLOR              = 0x050505;
constexpr uint32_t DESCRIPTION_TEXT_COLOR        = 0xE5E7EB;
constexpr uint32_t SECTION_SEPARATOR_COLOR       = 0x334155;

constexpr const char* EFFECT_TEXT = "DALI UI FOUNDATION";

// -----------------------------------------------------------------------------
// Common label / view helpers
// -----------------------------------------------------------------------------

Label CreateDescriptionLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(16.0f);
  label.SetTextColor(UiColor(DESCRIPTION_TEXT_COLOR));
  label.SetMultiLine(true);
  return label;
}

Label CreateLargeEffectLabel(float width, float height)
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

View CreateSectionSeparator()
{
  View separator = View::New();
  separator.SetRequestedWidth(MATCH_PARENT);
  separator.SetRequestedHeight(1.0f);
  separator.SetBackgroundColor(UiColor(SECTION_SEPARATOR_COLOR));
  separator.SetOpacity(0.35f);
  return separator;
}

// -----------------------------------------------------------------------------
// Gradient stop helpers
// -----------------------------------------------------------------------------

Dali::Vector<Gradient::StopNode> CreateBaseFillStops()
{
  Dali::Vector<Gradient::StopNode> stops;
  stops.PushBack(Gradient::StopNode(0.0f, UiColor(0.70f, 0.24f, 1.0f, 1.0f)));
  stops.PushBack(Gradient::StopNode(0.45f, UiColor(0.45f, 0.28f, 1.0f, 1.0f)));
  stops.PushBack(Gradient::StopNode(1.0f, UiColor(0.08f, 0.72f, 1.0f, 1.0f)));
  return stops;
}

Dali::Vector<Gradient::StopNode> CreateMaskEffectShimmerStops()
{
  Dali::Vector<Gradient::StopNode> stops;
  stops.PushBack(Gradient::StopNode(0.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  stops.PushBack(Gradient::StopNode(0.12f, UiColor(1.0f, 1.0f, 1.0f, 0.14f)));
  stops.PushBack(Gradient::StopNode(0.30f, UiColor(1.0f, 1.0f, 1.0f, 0.52f)));
  stops.PushBack(Gradient::StopNode(0.50f, UiColor(1.0f, 1.0f, 1.0f, 0.96f)));
  stops.PushBack(Gradient::StopNode(0.70f, UiColor(1.0f, 1.0f, 1.0f, 0.52f)));
  stops.PushBack(Gradient::StopNode(0.88f, UiColor(1.0f, 1.0f, 1.0f, 0.14f)));
  stops.PushBack(Gradient::StopNode(1.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  return stops;
}

Dali::Vector<Gradient::StopNode> CreateOverlayShimmerStops()
{
  Dali::Vector<Gradient::StopNode> stops;
  stops.PushBack(Gradient::StopNode(0.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  stops.PushBack(Gradient::StopNode(0.08f, UiColor(1.0f, 1.0f, 1.0f, 0.16f)));
  stops.PushBack(Gradient::StopNode(0.24f, UiColor(1.0f, 1.0f, 1.0f, 0.58f)));
  stops.PushBack(Gradient::StopNode(0.50f, UiColor(1.0f, 1.0f, 1.0f, 1.00f)));
  stops.PushBack(Gradient::StopNode(0.76f, UiColor(1.0f, 1.0f, 1.0f, 0.58f)));
  stops.PushBack(Gradient::StopNode(0.92f, UiColor(1.0f, 1.0f, 1.0f, 0.16f)));
  stops.PushBack(Gradient::StopNode(1.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  return stops;
}

Dali::Vector<Gradient::StopNode> CreateMinimalOverlayShimmerStops()
{
  Dali::Vector<Gradient::StopNode> stops;
  stops.PushBack(Gradient::StopNode(0.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  stops.PushBack(Gradient::StopNode(0.20f, UiColor(1.0f, 1.0f, 1.0f, 0.38f)));
  stops.PushBack(Gradient::StopNode(0.50f, UiColor(1.0f, 1.0f, 1.0f, 1.0f)));
  stops.PushBack(Gradient::StopNode(0.80f, UiColor(1.0f, 1.0f, 1.0f, 0.38f)));
  stops.PushBack(Gradient::StopNode(1.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)));
  return stops;
}

// -----------------------------------------------------------------------------
// Example 1. MaskEffect shimmer
// -----------------------------------------------------------------------------

/*
 * MaskEffect shimmer structure:
 *
 * Label
 *   - uses its glyph alpha as the mask
 *   - MaskEffect source: MaskEffectShimmerSource (AbsoluteLayout, not added to the scene)
 *
 * MaskEffectShimmerSource
 *   |- base GradientVisual          : fills the whole label area
 *   `- shimmerBand View             : child view moved horizontally
 *        `- shimmer GradientVisual  : transparent-white-transparent band
 *
 * The visible result is the MaskEffectShimmerSource clipped by the Label glyph alpha.
 * The shimmer is a real child View, and the GradientVisual inside that View
 * provides the bright band.
 */
class MaskEffectShimmerSource : public AbsoluteLayout
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
  MaskEffectShimmerSource() = default;

  static MaskEffectShimmerSource New()
  {
    MaskEffectShimmerSource source(AbsoluteLayout::New());
    source.Initialize();
    return source;
  }

  static MaskEffectShimmerSource DownCast(BaseHandle handle)
  {
    AbsoluteLayout layout = AbsoluteLayout::DownCast(handle);
    return layout && layout.GetAttachment<Data>(GetDataId()) ? MaskEffectShimmerSource(layout) : MaskEffectShimmerSource();
  }

  void ApplyTo(Label label)
  {
    if(label && *this && GetData())
    {
      // Label owns this view as its MaskEffect source; the source must not keep
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
    data->shimmerAnimation.SetLoopCount(Animation::INFINITE_LOOP);
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
  explicit MaskEffectShimmerSource(AbsoluteLayout layout)
  : AbsoluteLayout(layout)
  {
  }

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
    baseGradient.SetStopNodes(CreateBaseFillStops());

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
    Dali::Ui::Extension::View::SetSizeWidth(shimmerBand, SHIMMER_BAND_WIDTH);
    Dali::Ui::Extension::View::SetPositionX(shimmerBand, GetShimmerStartX());
    Dali::Ui::Extension::View::SetPositionY(shimmerBand, 0.0f);

    GradientVisual shimmerGradient = GradientVisual::New();
    shimmerGradient.SetLinearGradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    shimmerGradient.SetStopNodes(CreateMaskEffectShimmerStops());
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

// -----------------------------------------------------------------------------
// Example 2. TextGradient + TextGradientOverlay shimmer
// -----------------------------------------------------------------------------

/*
 * TextGradientOverlay shimmer structure:
 *
 * Label
 *   |- TextGradient                 : base glyph fill
 *   `- TextGradientOverlay          : transparent-white-transparent overlay gradient
 *        `- animated startOffset    : moves the sampled highlight in the shader
 *
 * No extra source View or shimmer View is created; no GradientVisual is used.
 * The "band" is only the bright region of the overlay gradient lookup texture.
 * The shimmer motion is produced by animating uTextGradientOverlayStartOffset.
 */
Gradient::Linear CreateTextGradientFill()
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  gradient.SetStopNodes(CreateBaseFillStops());
  return gradient;
}

Gradient::Linear CreateTextGradientOverlayShimmer(float startOffset)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  gradient.SetStartOffset(startOffset);
  gradient.SetStopNodes(CreateOverlayShimmerStops());
  return gradient;
}

Label CreateTextGradientOverlayShimmerLabel(float width, float height)
{
  Label label = CreateLargeEffectLabel(width, height);
  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradient(CreateTextGradientFill());
  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  return label;
}

// -----------------------------------------------------------------------------
// Example 3. Overlay-only minimal shimmer
// -----------------------------------------------------------------------------

/*
 * Small TextGradientOverlay shimmer sample:
 *
 * Label text color
 *   `- gray base glyph fill
 *
 * TextGradientOverlay
 *   `- transparent-white-transparent SCREEN highlight
 *        `- animated startOffset moves the highlight across the glyphs
 *
 * This is the minimal form of the overlay shimmer: no base TextGradient,
 * no MaskEffect source view, and no separate shimmer band actor.
 */
Label CreateOverlayOnlyShimmerLabel()
{
  struct AttachedAnimation
  {
    explicit AttachedAnimation(Animation animation)
    : animation(animation)
    {
    }

    // Keeps the looping overlay animation alive after the factory function returns.
    // The sample returns only the Label, so the animation handle follows the Label
    // lifetime through an attachment.
    Animation animation;
  };

  static AttachmentId attachedAnimationId = AttachmentId::Alloc();

  const float textFontSize      = 16.0f;
  const float animationDuration = 1.6f;
  const float startOffset       = 1.15f;
  const float endOffset         = -1.15f;

  Label label = Label::New("Thinking");
  label.SetFontFamily("SamsungOneUI_500");
  label.SetFontSize(textFontSize);
  label.SetTextColor(UiColor(0x777777));
  label.SetMultiLine(false);
  label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));

  Gradient::Linear overlay(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  overlay.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  overlay.SetSpreadMethod(Gradient::SpreadMethod::PAD);
  overlay.SetStartOffset(startOffset);
  overlay.SetStopNodes(CreateMinimalOverlayShimmerStops());

  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  label.SetTextGradientOverlay(overlay);

  Animation animation = Animation::New(animationDuration);
  label.Animate(animation)
    .TextGradientOverlayStartOffset(
      endOffset,
      Duration(animationDuration),
      AlphaFunction::LINEAR);
  animation.SetLoopCount(Animation::INFINITE_LOOP);
  animation.SetEndAction(Animation::DISCARD);
  animation.Play();

  label.SetAttachment(attachedAnimationId, Dali::MakeUnique<AttachedAnimation>(animation));
  return label;
}
} // namespace

// -----------------------------------------------------------------------------
// Application controller
// -----------------------------------------------------------------------------

class TextShimmerComparisonController : public ConnectionTracker
{
public:
  explicit TextShimmerComparisonController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextShimmerComparisonController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    auto positionSize = window.GetPositionSize();
    window.SetPositionSize(Dali::PositionSize(positionSize.x, positionSize.y, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(BACKGROUND_COLOR));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    root.SetBackgroundColor(UiColor(BACKGROUND_COLOR));

    root.Add(CreateDescriptionLabel("Gradient text using MaskEffect"));
    mMaskShimmerSource = MaskEffectShimmerSource::New();
    mMaskEffectLabel = CreateLargeEffectLabel(WRAP_CONTENT, WRAP_CONTENT);
    mMaskShimmerSource.ApplyTo(mMaskEffectLabel);
    root.Add(mMaskEffectLabel);

    root.Add(CreateSectionSeparator());

    root.Add(CreateDescriptionLabel("Gradient text using TextGradient + TextGradientOverlay"));
    mGradientOverlayLabel = CreateTextGradientOverlayShimmerLabel(WRAP_CONTENT, WRAP_CONTENT);
    root.Add(mGradientOverlayLabel);

    root.Add(CreateSectionSeparator());

    root.Add(CreateDescriptionLabel("Minimal shimmer using TextGradientOverlay only"));
    root.Add(CreateOverlayOnlyShimmerLabel());

    StartShimmer();

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextShimmerComparisonController::OnKeyEvent);
  }

  void SetEffectSize(float width, float height)
  {
    if(mMaskEffectLabel)
    {
      mMaskEffectLabel.SetRequestedWidth(width);
      mMaskEffectLabel.SetRequestedHeight(height);
    }

    if(mGradientOverlayLabel)
    {
      mGradientOverlayLabel.SetRequestedWidth(width);
      mGradientOverlayLabel.SetRequestedHeight(height);
    }
  }

  void StartShimmer()
  {
    mMaskShimmerSource.StartShimmer();
    StartGradientOverlayShimmer();
  }

  void StopShimmer()
  {
    mMaskShimmerSource.StopShimmer();
    StopGradientOverlayShimmer();
  }

  void StartGradientOverlayShimmer()
  {
    if(!mGradientOverlayLabel)
    {
      return;
    }

    StopGradientOverlayShimmer();

    mGradientOverlayLabel.SetTextGradientOverlay(
      CreateTextGradientOverlayShimmer(OVERLAY_SHIMMER_START_OFFSET));
    mGradientOverlayAnimation = Animation::New(OVERLAY_SHIMMER_DURATION_SECONDS);
    mGradientOverlayLabel.Animate(mGradientOverlayAnimation)
      .TextGradientOverlayStartOffset(
        OVERLAY_SHIMMER_END_OFFSET,
        Duration(OVERLAY_SHIMMER_DURATION_SECONDS),
        AlphaFunction::LINEAR);
    mGradientOverlayAnimation.SetLoopCount(Animation::INFINITE_LOOP);
    mGradientOverlayAnimation.SetEndAction(Animation::DISCARD);
    mGradientOverlayAnimation.Play();
  }

  void StopGradientOverlayShimmer()
  {
    if(mGradientOverlayAnimation)
    {
      mGradientOverlayAnimation.Stop();
      mGradientOverlayAnimation.Clear();
    }

    if(mGradientOverlayLabel)
    {
      mGradientOverlayLabel.SetTextGradientOverlay(Gradient::Base::None());
    }
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
  Application&             mApplication;
  Label                    mMaskEffectLabel;
  MaskEffectShimmerSource  mMaskShimmerSource;
  Label                    mGradientOverlayLabel;
  Animation                mGradientOverlayAnimation;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextShimmerComparisonController controller(application);
  application.MainLoop();

  return 0;
}
