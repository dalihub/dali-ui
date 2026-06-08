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
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/label.h>
#include <dali/devel-api/object/type-registry.h>

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
  impl->Initialize();
  return label;
}

Label Label::New(const Dali::String& text)
{
  Integration::LabelImplPtr impl = Integration::LabelImpl::New();

  Label label = Label(*impl);
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

void Label::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
}

Dali::String Label::GetText() const
{
  return GetImpl(*this).GetText();
}

void Label::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
}

Dali::String Label::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

void Label::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
}

float Label::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

void Label::SetMultiLine(bool multiLine)
{
  GetImpl(*this).SetMultiLine(multiLine);
}

bool Label::IsMultiLine() const
{
  return GetImpl(*this).IsMultiLine();
}

void Label::SetLineWrapMode(Text::LineWrapMode mode)
{
  GetImpl(*this).SetLineWrapMode(mode);
}

Text::LineWrapMode Label::GetLineWrapMode() const
{
  return GetImpl(*this).GetLineWrapMode();
}

void Label::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
}

UiColor Label::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

void Label::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
}

Text::Alignment Label::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

void Label::SetVerticalTextAlignment(Text::Alignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
}

Text::Alignment Label::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

void Label::SetOverflowMode(Text::OverflowMode mode)
{
  GetImpl(*this).SetOverflowMode(mode);
}

Text::OverflowMode Label::GetOverflowMode() const
{
  return GetImpl(*this).GetOverflowMode();
}

void Label::SetLineHeight(float lineHeight)
{
  GetImpl(*this).SetLineHeight(lineHeight);
}

float Label::GetLineHeight() const
{
  return GetImpl(*this).GetLineHeight();
}

void Label::SetLineHeightMode(Text::LineHeightMode mode)
{
  GetImpl(*this).SetLineHeightMode(mode);
}

Text::LineHeightMode Label::GetLineHeightMode() const
{
  return GetImpl(*this).GetLineHeightMode();
}

void Label::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetLayoutDirectionMode(mode);
}

Text::LayoutDirectionMode Label::GetLayoutDirectionMode() const
{
  return GetImpl(*this).GetLayoutDirectionMode();
}

void Label::SetMarkupEnabled(bool enabled)
{
  GetImpl(*this).SetMarkupEnabled(enabled);
}

bool Label::IsMarkupEnabled() const
{
  return GetImpl(*this).IsMarkupEnabled();
}

void Label::SetAnchorColor(const UiColor& color)
{
  GetImpl(*this).SetAnchorColor(color);
}

UiColor Label::GetAnchorColor()
{
  return GetImpl(*this).GetAnchorColor();
}

void Label::SetAnchorClickedColor(const UiColor& color)
{
  GetImpl(*this).SetAnchorClickedColor(color);
}

UiColor Label::GetAnchorClickedColor()
{
  return GetImpl(*this).GetAnchorClickedColor();
}

void Label::SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy policy)
{
  GetImpl(*this).SetMarqueeTriggerPolicy(policy);
}

Text::MarqueeTriggerPolicy Label::GetMarqueeTriggerPolicy() const
{
  return GetImpl(*this).GetMarqueeTriggerPolicy();
}

void Label::SetMarqueeSpeed(int speed)
{
  GetImpl(*this).SetMarqueeSpeed(speed);
}

int Label::GetMarqueeSpeed() const
{
  return GetImpl(*this).GetMarqueeSpeed();
}

void Label::SetMarqueeLoopCount(int loopCount)
{
  GetImpl(*this).SetMarqueeLoopCount(loopCount);
}

int Label::GetMarqueeLoopCount() const
{
  return GetImpl(*this).GetMarqueeLoopCount();
}

void Label::SetMarqueeLoopDelay(float delay)
{
  GetImpl(*this).SetMarqueeLoopDelay(delay);
}

float Label::GetMarqueeLoopDelay() const
{
  return GetImpl(*this).GetMarqueeLoopDelay();
}

void Label::SetMarqueeGap(int gap)
{
  GetImpl(*this).SetMarqueeGap(gap);
}

int Label::GetMarqueeGap() const
{
  return GetImpl(*this).GetMarqueeGap();
}

void Label::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  GetImpl(*this).SetMarqueeOrientation(orientation);
}

Text::MarqueeOrientation Label::GetMarqueeOrientation() const
{
  return GetImpl(*this).GetMarqueeOrientation();
}

void Label::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  GetImpl(*this).SetMarqueeStopMode(stopMode);
}

Text::MarqueeStopMode Label::GetMarqueeStopMode() const
{
  return GetImpl(*this).GetMarqueeStopMode();
}

void Label::SetFontWeight(Text::FontWeight weight)
{
  GetImpl(*this).SetFontWeight(weight);
}

Text::FontWeight Label::GetFontWeight() const
{
  return GetImpl(*this).GetFontWeight();
}

void Label::SetFontWidth(Text::FontWidth width)
{
  GetImpl(*this).SetFontWidth(width);
}

Text::FontWidth Label::GetFontWidth() const
{
  return GetImpl(*this).GetFontWidth();
}

void Label::SetFontSlant(Text::FontSlant slant)
{
  GetImpl(*this).SetFontSlant(slant);
}

Text::FontSlant Label::GetFontSlant() const
{
  return GetImpl(*this).GetFontSlant();
}

void Label::SetTextBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetTextBackgroundColor(color);
}

UiColor Label::GetTextBackgroundColor() const
{
  return GetImpl(*this).GetTextBackgroundColor();
}

void Label::ClearTextBackgroundColor()
{
  GetImpl(*this).ClearTextBackgroundColor();
}

void Label::SetUnderline(const Text::Underline& underline)
{
  GetImpl(*this).SetUnderline(underline);
}

void Label::ClearUnderline()
{
  return GetImpl(*this).ClearUnderline();
}

void Label::SetShadow(const Text::Shadow& shadow)
{
  GetImpl(*this).SetShadow(shadow);
}

void Label::ClearShadow()
{
  return GetImpl(*this).ClearShadow();
}

void Label::SetOutline(const Text::Outline& outline)
{
  GetImpl(*this).SetOutline(outline);
}

void Label::ClearOutline()
{
  return GetImpl(*this).ClearOutline();
}

void Label::SetLineThrough(const Text::LineThrough& lineThrough)
{
  GetImpl(*this).SetLineThrough(lineThrough);
}

void Label::ClearLineThrough()
{
  return GetImpl(*this).ClearLineThrough();
}

void Label::SetBevel(const Text::Bevel& bevel)
{
  GetImpl(*this).SetBevel(bevel);
}

void Label::ClearBevel()
{
  return GetImpl(*this).ClearBevel();
}

void Label::SetTextFit(const Text::FitRange& range)
{
  GetImpl(*this).SetTextFit(range);
}

void Label::SetTextFit(const Dali::Vector<Text::FitCandidate>& candidates)
{
  GetImpl(*this).SetTextFit(candidates);
}

void Label::ClearTextFit()
{
  GetImpl(*this).ClearTextFit();
}

void Label::SetFontSizeScale(float scale)
{
  GetImpl(*this).SetFontSizeScale(scale);
}

float Label::GetFontSizeScale() const
{
  return GetImpl(*this).GetFontSizeScale();
}

void Label::SetMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetMinimumFontSizeScale(scale);
}

float Label::GetMinimumFontSizeScale() const
{
  return GetImpl(*this).GetMinimumFontSizeScale();
}

void Label::SetMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetMaximumFontSizeScale(scale);
}

float Label::GetMaximumFontSizeScale() const
{
  return GetImpl(*this).GetMaximumFontSizeScale();
}

void Label::SetSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetSystemFontSizeScaleEnabled(enabled);
}

bool Label::IsSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsSystemFontSizeScaleEnabled();
}

void Label::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  GetImpl(*this).SetFontVariation(axes);
}

void Label::SetFontVariation(const Dali::String& settings)
{
  GetImpl(*this).SetFontVariation(settings);
}

Dali::Vector<Text::FontVariationAxis> Label::GetFontVariation() const
{
  return GetImpl(*this).GetFontVariation();
}

void Label::ClearFontVariation()
{
  GetImpl(*this).ClearFontVariation();
}

void Label::SetCutoutEnabled(bool enabled)
{
  GetImpl(*this).SetCutoutEnabled(enabled);
}

bool Label::IsCutoutEnabled() const
{
  return GetImpl(*this).IsCutoutEnabled();
}

void Label::SetMaskEffect(View view)
{
  GetImpl(*this).SetMaskEffect(view);
}

void Label::ClearMaskEffect()
{
  GetImpl(*this).ClearMaskEffect();
}

void Label::SetAsyncRendering(bool asyncRendering)
{
  GetImpl(*this).SetAsyncRendering(asyncRendering);
}

bool Label::IsAsyncRendering() const
{
  return GetImpl(*this).IsAsyncRendering();
}

void Label::SetRenderScale(float scale)
{
  GetImpl(*this).SetRenderScale(scale);
}

float Label::GetRenderScale() const
{
  return GetImpl(*this).GetRenderScale();
}

void Label::SetTranslatableText(StringView resourceId)
{
  GetImpl(*this).SetTranslatableText(resourceId);
}

void Label::SetTranslatableText(StringView resourceId, StringView domain)
{
  GetImpl(*this).SetTranslatableText(resourceId, domain);
}

Dali::String Label::GetTranslatableText() const
{
  return GetImpl(*this).GetTranslatableText();
}

void Label::ClearTranslatableText()
{
  GetImpl(*this).ClearTranslatableText();
}
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
