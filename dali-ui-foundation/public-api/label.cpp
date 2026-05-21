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

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/label.h>

namespace Dali
{

namespace Ui
{

inline Integration::LabelImpl& GetImpl(Label& label)
{
  DALI_ASSERT_ALWAYS(label);

  Dali::RefObject& handle = label.GetImplementation();

  return static_cast<Integration::LabelImpl&>(handle);
}

inline const Integration::LabelImpl& GetImpl(const Label& label)
{
  DALI_ASSERT_ALWAYS(label);

  const Dali::RefObject& handle = label.GetImplementation();

  return static_cast<const Integration::LabelImpl&>(handle);
}

Label::Label()
{
}

Label Label::New()
{
  Integration::LabelImplPtr impl = Integration::LabelImpl::New();

  Label label = Label(*impl);

  // Second-phase initialization
  impl->Initialize();
  return label;
}

Label Label::New(const Dali::String& text)
{
  Integration::LabelImplPtr impl = Integration::LabelImpl::New();

  Label label = Label(*impl);

  // Second-phase initialization
  impl->Initialize();
  label.SetText(text);
  return label;
}

Label::Label(const Label& label)
: View(label)
{
}

Label::Label(Label&& rhs) noexcept
: View(std::move(rhs))
{
}

Label::~Label()
{
}

Label& Label::operator=(const Label& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

Label& Label::operator=(Label&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

Label Label::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<Label, Integration::LabelImpl>(handle);
}

Label::Label(Integration::LabelImpl& implementation)
: View(implementation)
{
}

Label::Label(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::LabelImpl>(internal);
}

// =============================================================================
// Properties
// =============================================================================

Label& Label::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
  return *this;
}

Dali::String Label::GetText() const
{
  return GetImpl(*this).GetText();
}

Label& Label::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
  return *this;
}

Dali::String Label::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

Label& Label::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
  return *this;
}

float Label::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

Label& Label::SetMultiLine(bool multiLine)
{
  GetImpl(*this).SetMultiLine(multiLine);
  return *this;
}

bool Label::IsMultiLine() const
{
  return GetImpl(*this).IsMultiLine();
}

Label& Label::SetLineWrapMode(Text::LineWrapMode mode)
{
  GetImpl(*this).SetLineWrapMode(mode);
  return *this;
}

Text::LineWrapMode Label::GetLineWrapMode() const
{
  return GetImpl(*this).GetLineWrapMode();
}

Label& Label::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

UiColor Label::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

Label& Label::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
  return *this;
}

Text::Alignment Label::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

Label& Label::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
  return *this;
}

Text::Alignment Label::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

Label& Label::SetOverflowMode(Text::OverflowMode mode)
{
  GetImpl(*this).SetOverflowMode(mode);
  return *this;
}

Text::OverflowMode Label::GetOverflowMode() const
{
  return GetImpl(*this).GetOverflowMode();
}

Label& Label::SetLineHeight(float lineHeight)
{
  GetImpl(*this).SetLineHeight(lineHeight);
  return *this;
}

float Label::GetLineHeight() const
{
  return GetImpl(*this).GetLineHeight();
}

Label& Label::SetLineHeightMode(Text::LineHeightMode mode)
{
  GetImpl(*this).SetLineHeightMode(mode);
  return *this;
}

Text::LineHeightMode Label::GetLineHeightMode() const
{
  return GetImpl(*this).GetLineHeightMode();
}

Label& Label::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
  return *this;
}

Text::LayoutDirectionMode Label::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

Label& Label::SetMarkupEnabled(bool enabled)
{
  GetImpl(*this).SetMarkupEnabled(enabled);
  return *this;
}

bool Label::IsMarkupEnabled() const
{
  return GetImpl(*this).IsMarkupEnabled();
}

Label& Label::SetAnchorColor(const UiColor& color)
{
  GetImpl(*this).SetAnchorColor(color);
  return *this;
}

UiColor Label::GetAnchorColor()
{
  return GetImpl(*this).GetAnchorColor();
}

Label& Label::SetAnchorClickedColor(const UiColor& color)
{
  GetImpl(*this).SetAnchorClickedColor(color);
  return *this;
}

UiColor Label::GetAnchorClickedColor()
{
  return GetImpl(*this).GetAnchorClickedColor();
}

Label& Label::SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy policy)
{
  GetImpl(*this).SetMarqueeTriggerPolicy(policy);
  return *this;
}

Text::MarqueeTriggerPolicy Label::GetMarqueeTriggerPolicy() const
{
  return GetImpl(*this).GetMarqueeTriggerPolicy();
}

Label& Label::SetMarqueeSpeed(int speed)
{
  GetImpl(*this).SetMarqueeSpeed(speed);
  return *this;
}

int Label::GetMarqueeSpeed() const
{
  return GetImpl(*this).GetMarqueeSpeed();
}

Label& Label::SetMarqueeLoopCount(int loopCount)
{
  GetImpl(*this).SetMarqueeLoopCount(loopCount);
  return *this;
}

int Label::GetMarqueeLoopCount() const
{
  return GetImpl(*this).GetMarqueeLoopCount();
}

Label& Label::SetMarqueeLoopDelay(float delay)
{
  GetImpl(*this).SetMarqueeLoopDelay(delay);
  return *this;
}

float Label::GetMarqueeLoopDelay() const
{
  return GetImpl(*this).GetMarqueeLoopDelay();
}

Label& Label::SetMarqueeGap(int gap)
{
  GetImpl(*this).SetMarqueeGap(gap);
  return *this;
}

int Label::GetMarqueeGap() const
{
  return GetImpl(*this).GetMarqueeGap();
}

Label& Label::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  GetImpl(*this).SetMarqueeOrientation(orientation);
  return *this;
}

Text::MarqueeOrientation Label::GetMarqueeOrientation() const
{
  return GetImpl(*this).GetMarqueeOrientation();
}

Label& Label::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  GetImpl(*this).SetMarqueeStopMode(stopMode);
  return *this;
}

Text::MarqueeStopMode Label::GetMarqueeStopMode() const
{
  return GetImpl(*this).GetMarqueeStopMode();
}

Label& Label::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
  return *this;
}

Text::FontWeight Label::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

Label& Label::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
  return *this;
}

Text::FontWidth Label::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

Label& Label::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
  return *this;
}

Text::FontSlant Label::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

Label& Label::SetTextBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetTextBackgroundColor(color);
  return *this;
}

UiColor Label::GetTextBackgroundColor() const
{
  return GetImpl(*this).GetTextBackgroundColor();
}

void Label::ClearTextBackgroundColor()
{
  GetImpl(*this).ClearTextBackgroundColor();
}

Label& Label::SetUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetUnderline(underline);
  return *this;
}

void Label::ClearUnderline()
{
  return GetImpl(*this).ClearUnderline();
}

Label& Label::SetShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetShadow(shadow);
  return *this;
}

void Label::ClearShadow()
{
  return GetImpl(*this).ClearShadow();
}

Label& Label::SetOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetOutline(outline);
  return *this;
}

void Label::ClearOutline()
{
  return GetImpl(*this).ClearOutline();
}

Label& Label::SetLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetLineThrough(lineThrough);
  return *this;
}

void Label::ClearLineThrough()
{
  return GetImpl(*this).ClearLineThrough();
}

Label& Label::SetBevel(const Text::Bevel& bevel)
{
  GetImpl(*this).SetBevel(bevel);
  return *this;
}

void Label::ClearBevel()
{
  return GetImpl(*this).ClearBevel();
}

Label& Label::SetTextFit(const Text::FitRange& range)
{
  GetImpl(*this).SetTextFit(range);
  return *this;
}

Label& Label::SetTextFit(const Dali::Vector<Text::FitCandidate>& candidates)
{
  GetImpl(*this).SetTextFit(candidates);
  return *this;
}

void Label::ClearTextFit()
{
  GetImpl(*this).ClearTextFit();
}

Label& Label::SetFontSizeScale(float scale)
{
  GetImpl(*this).SetFontSizeScale(scale);
  return *this;
}

float Label::GetFontSizeScale() const
{
  return GetImpl(*this).GetFontSizeScale();
}

Label& Label::SetMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetMinimumFontSizeScale(scale);
  return *this;
}

float Label::GetMinimumFontSizeScale() const
{
  return GetImpl(*this).GetMinimumFontSizeScale();
}

Label& Label::SetMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetMaximumFontSizeScale(scale);
  return *this;
}

float Label::GetMaximumFontSizeScale() const
{
  return GetImpl(*this).GetMaximumFontSizeScale();
}

Label& Label::SetSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetSystemFontSizeScaleEnabled(enabled);
  return *this;
}

bool Label::IsSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsSystemFontSizeScaleEnabled();
}

Label& Label::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  GetImpl(*this).SetFontVariation(axes);
  return *this;
}

Label& Label::SetFontVariation(const Dali::String& settings)
{
  GetImpl(*this).SetFontVariation(settings);
  return *this;
}

Dali::Vector<Text::FontVariationAxis> Label::GetFontVariation() const
{
  return GetImpl(*this).GetFontVariation();
}

void Label::ClearFontVariation()
{
  GetImpl(*this).ClearFontVariation();
}

Label& Label::SetCutoutEnabled(bool enabled)
{
  GetImpl(*this).SetCutoutEnabled(enabled);
  return *this;
}

bool Label::IsCutoutEnabled() const
{
  return GetImpl(*this).IsCutoutEnabled();
}

Label& Label::SetMaskEffect(View view)
{
  GetImpl(*this).SetMaskEffect(view);
  return *this;
}

void Label::ClearMaskEffect()
{
  GetImpl(*this).ClearMaskEffect();
}

Label& Label::SetAsyncRendering(bool asyncRendering)
{
  GetImpl(*this).SetAsyncRendering(asyncRendering);
  return *this;
}

bool Label::IsAsyncRendering() const
{
  return GetImpl(*this).IsAsyncRendering();
}

Label& Label::SetRenderScale(float scale)
{
  GetImpl(*this).SetRenderScale(scale);
  return *this;
}

float Label::GetRenderScale() const
{
  return GetImpl(*this).GetRenderScale();
}

Label& Label::SetTranslatableText(StringView resourceId)
{
  GetImpl(*this).SetTranslatableText(resourceId);
  return *this;
}

Label& Label::SetTranslatableText(StringView resourceId, StringView domain)
{
  GetImpl(*this).SetTranslatableText(resourceId, domain);
  return *this;
}

Dali::String Label::GetTranslatableText() const
{
  return GetImpl(*this).GetTranslatableText();
}

void Label::ClearTranslatableText()
{
  GetImpl(*this).ClearTranslatableText();
}

// =============================================================================
// Read Only
// =============================================================================
int Label::GetLineCount()
{
  return GetImpl(*this).GetLineCount();
}

int Label::GetLineCount(float width)
{
  return GetImpl(*this).GetLineCount(width);
}

int Label::GetAsyncLineCount() const
{
  return GetImpl(*this).GetAsyncLineCount();
}

bool Label::IsMarqueeRunning() const
{
  return GetImpl(*this).IsMarqueeRunning();
}

float Label::GetAdjustedFontSizeScale() const
{
  return GetImpl(*this).GetAdjustedFontSizeScale();
}

// =============================================================================
// Method
// =============================================================================
void Label::StartMarquee()
{
  GetImpl(*this).StartMarquee();
}

void Label::StopMarquee()
{
  GetImpl(*this).StopMarquee();
}

void Label::RequestAsyncNaturalSize()
{
  GetImpl(*this).RequestAsyncNaturalSize();
}

void Label::RequestAsyncHeightForWidth(float width)
{
  GetImpl(*this).RequestAsyncHeightForWidth(width);
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View, const Dali::String&)>& Label::AnchorClickedSignal()
{
  return GetImpl(*this).AnchorClickedSignal();
}

Signal<void(View, float, float)>& Label::AsyncRenderFinishedSignal()
{
  return GetImpl(*this).AsyncRenderFinishedSignal();
}

Signal<void(View, float, float)>& Label::AsyncNaturalSizeComputedSignal()
{
  return GetImpl(*this).AsyncNaturalSizeComputedSignal();
}

Signal<void(View, float, float)>& Label::AsyncHeightForWidthComputedSignal()
{
  return GetImpl(*this).AsyncHeightForWidthComputedSignal();
}

LabelAnimationBridge Label::Animate(Animation animation)
{
  return LabelAnimationBridge(animation, *this);
}

LabelAnimationSpec Label::NewAnimationSpec()
{
  return LabelAnimationSpec::New();
}

} // namespace Ui

} // namespace Dali
