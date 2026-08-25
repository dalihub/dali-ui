#pragma once

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
#include <dali/integration-api/system/system-settings.h>
#include <dali/public-api/object/weak-handle.h>
#include <cstdint>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/size-negotiated-view-impl.h>
#include <dali-ui-foundation/integration-api/text/async-text-interface.h>
#include <dali-ui-foundation/integration-api/text/text-anchor-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-control-interface.h>
#include <dali-ui-foundation/integration-api/text/text-scroller-interface.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{
namespace Text
{
struct TextGradientPropertyData;
using TextGradientPropertyDataPtr = std::unique_ptr<TextGradientPropertyData>;

struct TextRevealData;
using TextRevealDataPtr = std::unique_ptr<TextRevealData>;

class StyledTextSourceData;
using StyledTextSourceDataPtr = std::unique_ptr<StyledTextSourceData>;
} // namespace Text
} // namespace Internal

namespace Text
{
struct AsyncTextParameters;
class Controller;
class TextScroller;

using ControllerPtr   = IntrusivePtr<Controller>;
using TextScrollerPtr = IntrusivePtr<TextScroller>;
} // namespace Text

namespace Integration
{

class LabelAccessible;
class LabelImpl;
using LabelImplPtr = IntrusivePtr<LabelImpl>;

/**
 * @brief This is the internal implementation class for Label.
 *
 * @see Dali::Ui::LabelImpl
 */
class DALI_UI_API LabelImpl : public SizeNegotiatedViewImpl,
                              public Text::ControlInterface,
                              public Text::ScrollerInterface,
                              public Text::AnchorControlInterface,
                              public Text::AsyncTextInterface
{
  friend class LabelAccessible;

public:
  // Creation & Destruction

  /**
   * @brief Creates a new Label.
   */
  static LabelImplPtr New();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LabelImpl();

public:
  // API

  /**
   * @copydoc Dali::Ui::Label::SetText
   */
  void SetText(const Dali::String& text);

  /**
   * @copydoc Dali::Ui::Label::GetText
   */
  Dali::String GetText() const;

  /**
   * @copydoc Dali::Ui::Label::SetStyledText
   */
  void SetStyledText(const Ui::Text::StyledText& styledText);

  /**
   * @copydoc Dali::Ui::Label::GetStyledText
   */
  Ui::Text::StyledText GetStyledText() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontFamily
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @copydoc Dali::Ui::Label::GetFontFamily
   */
  Dali::String GetFontFamily() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontSize
   */
  void SetFontSize(float fontSize);

  /**
   * @copydoc Dali::Ui::Label::GetFontSize
   */
  float GetFontSize() const;

  /**
   * @copydoc Dali::Ui::Label::SetMultiLine
   */
  void SetMultiLine(bool multiLine);

  /**
   * @copydoc Dali::Ui::Label::IsMultiLine
   */
  bool IsMultiLine() const;

  /**
   * @copydoc Dali::Ui::Label::SetLineWrapMode
   */
  void SetLineWrapMode(Ui::Text::LineWrapMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLineWrapMode
   */
  Ui::Text::LineWrapMode GetLineWrapMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextColor
   */
  void SetTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::Label::GetTextColor
   */
  UiColor GetTextColor();

  /**
   * @copydoc Dali::Ui::Label::SetTextGradient
   */
  void SetTextGradient(const Gradient::Base& gradient);

  /**
   * @copydoc Dali::Ui::Label::GetTextGradient
   */
  Gradient::Base GetTextGradient() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextGradientBoundsMode
   */
  void SetTextGradientBoundsMode(Ui::Text::GradientBoundsMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetTextGradientBoundsMode
   */
  Ui::Text::GradientBoundsMode GetTextGradientBoundsMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextGradientOverlay
   */
  void SetTextGradientOverlay(const Gradient::Base& gradient);

  /**
   * @copydoc Dali::Ui::Label::GetTextGradientOverlay
   */
  Gradient::Base GetTextGradientOverlay() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextGradientOverlayBoundsMode
   */
  void SetTextGradientOverlayBoundsMode(Ui::Text::GradientBoundsMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetTextGradientOverlayBoundsMode
   */
  Ui::Text::GradientBoundsMode GetTextGradientOverlayBoundsMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextGradientOverlayMode
   */
  void SetTextGradientOverlayMode(Ui::Text::GradientOverlayMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetTextGradientOverlayMode
   */
  Ui::Text::GradientOverlayMode GetTextGradientOverlayMode() const;

  /**
   * @brief Lazily registers the animatable TextGradient start offset source property.
   *
   * The property is registered on the Label actor using the same name as the
   * renderer uniform, but it is not exposed through Label::Property.
   *
   * @return The registered property index.
   */
  Dali::Property::Index EnsureGradientAnimOffset();

  /**
   * @brief Lazily registers the animatable TextGradientOverlay start offset source property.
   *
   * The property is registered on the Label actor using the same name as the
   * renderer uniform, but it is not exposed through Label::Property.
   *
   * @return The registered property index.
   */
  Dali::Property::Index EnsureGradientOverlayAnimOffset();

  /**
   * @copydoc Dali::Ui::Label::SetTextReveal
   */
  void SetTextReveal(const Ui::Text::Reveal& reveal);

  /**
   * @copydoc Dali::Ui::Label::GetTextReveal
   */
  Ui::Text::Reveal GetTextReveal() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextRevealProgress
   */
  void SetTextRevealProgress(float progress);

  /**
   * @copydoc Dali::Ui::Label::GetTextRevealProgress
   */
  float GetTextRevealProgress() const;

  /**
   * @brief Returns the stable scene property used to animate reveal progress.
   *
   * The property is registered lazily and is never replaced by reveal
   * configuration, text, or synchronous/asynchronous rendering changes.
   *
   * @return The registered reveal progress property index.
   */
  Property::Index EnsureTextRevealProgress();

  /**
   * @copydoc Dali::Ui::Label::SetHorizontalTextAlignment
   */
  void SetHorizontalTextAlignment(Ui::Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::Label::GetHorizontalTextAlignment
   */
  Ui::Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::Label::SetVerticalTextAlignment
   */
  void SetVerticalTextAlignment(Ui::Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::Label::GetVerticalTextAlignment
   */
  Ui::Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextOverflowMode
   */
  void SetTextOverflowMode(Ui::Text::OverflowMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetTextOverflowMode
   */
  Ui::Text::OverflowMode GetTextOverflowMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetLineHeight
   */
  void SetLineHeight(float lineHeight);

  /**
   * @copydoc Dali::Ui::Label::GetLineHeight
   */
  float GetLineHeight() const;

  /**
   * @copydoc Dali::Ui::Label::SetLineHeightMode
   */
  void SetLineHeightMode(Ui::Text::LineHeightMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLineHeightMode
   */
  Ui::Text::LineHeightMode GetLineHeightMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetLayoutDirectionMode
   */
  void SetLayoutDirectionMode(Ui::Text::LayoutDirectionMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLayoutDirectionMode
   */
  Ui::Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetAnchorColor
   */
  void SetAnchorColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::Label::GetAnchorColor
   */
  UiColor GetAnchorColor();

  /**
   * @copydoc Dali::Ui::Label::SetAnchorClickedColor
   */
  void SetAnchorClickedColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::Label::GetAnchorClickedColor
   */
  UiColor GetAnchorClickedColor();

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeTriggerPolicy
   */
  void SetMarqueeTriggerPolicy(Ui::Text::MarqueeTriggerPolicy policy);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeTriggerPolicy
   */
  Ui::Text::MarqueeTriggerPolicy GetMarqueeTriggerPolicy() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeSpeed
   */
  void SetMarqueeSpeed(int speed);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeSpeed
   */
  int GetMarqueeSpeed() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeLoopCount
   */
  void SetMarqueeLoopCount(int loopCount);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeLoopCount
   */
  int GetMarqueeLoopCount() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeLoopDelay
   */
  void SetMarqueeLoopDelay(float delay);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeLoopDelay
   */
  float GetMarqueeLoopDelay() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeGap
   */
  void SetMarqueeGap(int gap);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeGap
   */
  int GetMarqueeGap() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeOrientation
   */
  void SetMarqueeOrientation(Ui::Text::MarqueeOrientation orientation);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeOrientation
   */
  Ui::Text::MarqueeOrientation GetMarqueeOrientation() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeStopMode
   */
  void SetMarqueeStopMode(Ui::Text::MarqueeStopMode stopMode);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeStopMode
   */
  Ui::Text::MarqueeStopMode GetMarqueeStopMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontWeight
   */
  void SetFontWeight(Ui::Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::Label::GetFontWeight
   */
  Ui::Text::FontWeight GetFontWeight() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontWidth
   */
  void SetFontWidth(Ui::Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::Label::GetFontWidth
   */
  Ui::Text::FontWidth GetFontWidth() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontSlant
   */
  void SetFontSlant(Ui::Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::Label::GetFontSlant
   */
  Ui::Text::FontSlant GetFontSlant() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextBackgroundColor
   */
  void SetTextBackgroundColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::Label::GetTextBackgroundColor
   */
  UiColor GetTextBackgroundColor() const;

  /**
   * @copydoc Dali::Ui::Label::ClearTextBackgroundColor
   */
  void ClearTextBackgroundColor();

  /**
   * @copydoc Dali::Ui::Label::SetTextUnderline
   */
  void SetTextUnderline(const Ui::Text::Underline& underline);

  /**
   * @copydoc Dali::Ui::Label::GetTextUnderline
   */
  Ui::Text::Underline GetTextUnderline() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextShadow
   */
  void SetTextShadow(const Ui::Text::Shadow& shadow);

  /**
   * @copydoc Dali::Ui::Label::GetTextShadow
   */
  Ui::Text::Shadow GetTextShadow() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextOutline
   */
  void SetTextOutline(const Ui::Text::Outline& outline);

  /**
   * @copydoc Dali::Ui::Label::GetTextOutline
   */
  Ui::Text::Outline GetTextOutline() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextLineThrough
   */
  void SetTextLineThrough(const Ui::Text::LineThrough& lineThrough);

  /**
   * @copydoc Dali::Ui::Label::GetTextLineThrough
   */
  Ui::Text::LineThrough GetTextLineThrough() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextBevel
   */
  void SetTextBevel(const Ui::Text::Bevel& bevel);

  /**
   * @copydoc Dali::Ui::Label::GetTextBevel
   */
  Ui::Text::Bevel GetTextBevel() const;

  /**
   * @see Dali::Ui::Label::SetTextFit
   */
  void SetTextFit(const Ui::Text::Fit& fit);

  /**
   * @see Dali::Ui::Label::SetTextFit
   */
  void SetTextFit(const Ui::Text::Fit::Range& range);

  /**
   * @see Dali::Ui::Label::SetTextFit(const Dali::Vector<Ui::Text::Fit::Candidate>&)
   */
  void SetTextFit(const Dali::Vector<Ui::Text::Fit::Candidate>& candidates);

  /**
   * @copydoc Dali::Ui::Label::GetTextFit
   */
  Ui::Text::Fit GetTextFit() const;

  /**
   * @brief Sets the explicit font size scale used internally.
   */
  void SetFontSizeScale(float scale);

  /**
   * @brief Gets the explicit font size scale used internally.
   */
  float GetFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::Label::SetMinimumFontSizeScale
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::Label::GetMinimumFontSizeScale
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::Label::SetMaximumFontSizeScale
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::Label::GetMaximumFontSizeScale
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @copydoc Dali::Ui::Label::SetSystemFontSizeScaleEnabled
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::Label::IsSystemFontSizeScaleEnabled
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @see Dali::Ui::Label::SetFontVariation
   */
  void SetFontVariation(const Dali::Vector<Ui::Text::FontVariation::Axis>& axes);

  /**
   * @see Dali::Ui::Label::SetFontVariation(const Dali::String&)
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @copydoc Dali::Ui::Label::GetFontVariation
   */
  Dali::Vector<Ui::Text::FontVariation::Axis> GetFontVariation() const;

  /**
   * @brief Registers or retrieves a font variation property for the given axis tag.
   *
   * If a property with the same tag is already registered, the existing property
   * is reused and its current value is updated.
   *
   * @param[in] tag A 4-character OpenType variation axis tag.
   * @return The property index associated with the variation tag,
   *         or Property::INVALID_INDEX on failure.
   */
  Dali::Property::Index RegisterFontVariationProperty(const Dali::String& tag);

  /**
   * @copydoc Dali::Ui::Label::SetTextCutoutEnabled
   */
  void SetTextCutoutEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::Label::IsTextCutoutEnabled
   */
  bool IsTextCutoutEnabled() const;

  /**
   * @brief Sets the additional spacing between letters in pixels.
   *
   * Positive values increase the spacing, while negative values reduce it.
   *
   * @param[in] spacing The additional letter spacing in pixels.
   */
  void SetLetterSpacing(float spacing);

  /**
   * @brief Gets the additional spacing between letters in pixels.
   *
   * @return The additional letter spacing in pixels.
   */
  float GetLetterSpacing() const;

  /**
   * @copydoc Dali::Ui::Label::SetMaskEffect
   */
  void SetMaskEffect(View view);

  /**
   * @copydoc Dali::Ui::Label::ClearMaskEffect
   */
  void ClearMaskEffect();

  /**
   * @copydoc Dali::Ui::Label::SetAsyncRendering
   */
  void SetAsyncRendering(bool asyncRendering);

  /**
   * @copydoc Dali::Ui::Label::IsAsyncRendering
   */
  bool IsAsyncRendering() const;

  /**
   * @copydoc Dali::Ui::Label::SetRenderScale
   */
  void SetRenderScale(float scale);

  /**
   * @copydoc Dali::Ui::Label::GetRenderScale
   */
  float GetRenderScale() const;

  /**
   * @see Dali::Ui::Label::SetTranslatableText
   */
  void SetTranslatableText(StringView resourceId);

  /**
   * @see Dali::Ui::Label::SetTranslatableText(StringView resourceId, StringView domain);
   */
  void SetTranslatableText(StringView resourceId, StringView domain);

  /**
   * @copydoc Dali::Ui::Label::GetTranslatableText
   */
  Dali::String GetTranslatableText() const;

  /**
   * @copydoc Dali::Ui::Label::ClearTranslatableText
   */
  void ClearTranslatableText();

  // Read Only
  /**
   * @see Dali::Ui::Label::GetLineCount
   */
  int GetLineCount();

  /**
   * @see Dali::Ui::Label::GetLineCount(float)
   */
  int GetLineCount(float width);

  /**
   * @see Dali::Ui::Label::GetAsyncLineCount
   */
  int GetAsyncLineCount() const;

  /**
   * @copydoc Dali::Ui::Label::IsMarqueeRunning
   */
  bool IsMarqueeRunning() const;

  /**
   * @copydoc Dali::Ui::Label::GetAdjustedFontSizeScale
   */
  float GetAdjustedFontSizeScale() const;

  // Method
  /**
   * @copydoc Dali::Ui::Label::StartMarquee
   */
  void StartMarquee();

  /**
   * @copydoc Dali::Ui::Label::StopMarquee
   */
  void StopMarquee();

  /**
   * @copydoc Dali::Ui::Label::SetPixelSnapFactor
   */
  void SetPixelSnapFactor(float factor);

  /**
   * @copydoc Dali::Ui::Label::GetPixelSnapFactor
   */
  float GetPixelSnapFactor() const;

  /**
   * @copydoc Dali::Ui::Label::RequestAsyncNaturalSize
   */
  void RequestAsyncNaturalSize();

  /**
   * @copydoc Dali::Ui::Label::RequestAsyncHeightForWidth
   */
  void RequestAsyncHeightForWidth(float width);

  // Integration-only
  /**
   * @brief Requests asynchronous text rendering with a fixed size.
   *
   * @param[in] width The width used for rendering.
   * @param[in] height The height used for rendering.
   */
  void RequestAsyncRenderWithFixedSize(float width, float height);

  /**
   * @brief Requests asynchronous text rendering with a fixed width.
   *
   * @param[in] width The width used for rendering.
   * @param[in] heightConstraint The maximum available height.
   */
  void RequestAsyncRenderWithFixedWidth(float width, float heightConstraint);

  /**
   * @brief Requests asynchronous text rendering with a fixed height.
   *
   * @param[in] widthConstraint The maximum available width.
   * @param[in] height The height used for rendering.
   */
  void RequestAsyncRenderWithFixedHeight(float widthConstraint, float height);

  /**
   * @brief Requests asynchronous text rendering within the given constraints.
   *
   * @param[in] widthConstraint The maximum available width.
   * @param[in] heightConstraint The maximum available height.
   */
  void RequestAsyncRenderWithConstraints(float widthConstraint, float heightConstraint);

public: // Signals
  /**
   * @copydoc Dali::Ui::Label::AnchorClickedSignal()
   */
  Signal<void(View, const Dali::String&)>& AnchorClickedSignal();

  /**
   * @copydoc Dali::Ui::Label::AsyncRenderFinishedSignal()
   */
  Signal<void(View, float, float)>& AsyncRenderFinishedSignal();

  /**
   * @copydoc Dali::Ui::Label::AsyncNaturalSizeComputedSignal()
   */
  Signal<void(View, float, float)>& AsyncNaturalSizeComputedSignal();

  /**
   * @copydoc Dali::Ui::Label::AsyncHeightForWidthComputedSignal()
   */
  Signal<void(View, float, float)>& AsyncHeightForWidthComputedSignal();

protected:
  // Construction

  /**
   * @brief LabelImpl constructor.
   */
  LabelImpl();

private: // Config
  /**
   * @brief Applies default values from UiConfig if applied.
   */
  void ApplyInitialConfig();

private: // UiScale
  /**
   * @brief Sets the UI scale used for text-specific metrics.
   *
   * This scale is used for text-specific size calculation, such as font size.
   *
   * @param[in] scale The text UI scale.
   * @return True if the scale was changed, false otherwise.
   */
  bool SetTextUiScale(float scale);

  /**
   * @brief Gets the UI scale used for text-specific metrics.
   *
   * @return The text UI scale.
   */
  float GetTextUiScale() const;

  /**
   * @brief Gets the effective padding used for text layout.
   *
   * The view padding is adjusted by the current text UI scale.
   *
   * @return The effective text padding.
   */
  Insets GetEffectiveTextPadding() const;

private: // System FontSize
  /**
   * @brief Applies the current platform font size preference.
   *
   * @param[in] fontSize The platform font size preference.
   */
  void ApplySystemFontSize(Dali::Integration::SystemSettings::FontSize fontSize);

  /**
   * @brief Called when the platform font size preference changes.
   *
   * @param[in] fontSize The changed platform font size preference.
   */
  void OnSystemFontSizeChanged(Dali::Integration::SystemSettings::FontSize fontSize);

public: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

public: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc ViewImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc SizeNegotiatedViewImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc SizeNegotiatedViewImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

protected: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  LayoutRect OnArrange(const LayoutRect& bounds) override;

public: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnAnimateAnimatableProperty()
   */
  void OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index,
                                   Dali::Animation::State state) override;

  /**
   * @copydoc CustomActorImpl::OnConstraintAnimatableProperty()
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied) override;

private: // From ControlInterface
  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

  /**
   * @copydoc Text::ControlInterface::InvalidateTextMeasure()
   */
  void InvalidateTextMeasure() override;

  /**
   * @copydoc Text::ControlInterface::RequestAsyncRender()
   */
  void RequestAsyncRender() override;

private: // From ScrollerInterface
  /**
   * @copydoc Text::ScrollerInterface::ScrollingFinished()
   */
  void ScrollingFinished() override;

private: // From AnchorControlInterface
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  bool AnchorClicked(uint32_t cursorPosition, std::string& href) override;

  /**
   * @copydoc Text::AnchorControlInterface::EmitAnchorClicked()
   */
  void EmitAnchorClicked(const std::string& href) override;

private: // From AsyncTextInterface
  /**
   * @copydoc Text::AsyncTextInterface::AsyncInitializeMarquee()
   */
  void AsyncInitializeMarquee(const Ui::Text::AsyncTextRenderInfo& renderInfo) override;

  /**
   * @copydoc Text::AsyncTextInterface::AsyncTextFitChanged()
   */
  void AsyncTextFitChanged(float pointSize) override;

  /**
   * @copydoc Text::AsyncTextInterface::AsyncRenderFinished()
   */
  void AsyncRenderFinished(Ui::Text::AsyncTextRenderInfo&& renderInfo) override;

  /**
   * @copydoc Text::AsyncTextInterface::AsyncSizeComputed()
   */
  void AsyncSizeComputed(const Ui::Text::AsyncTextRenderInfo& renderInfo) override;

private: // Implementation
  struct InlineReplacementUpdateData;

  /**
   * @brief Marks that the text renderer needs to be updated on the next relayout.
   */
  void RequestRendererUpdate();

  /**
   * @brief Checks whether valid inline replacement source data is available.
   *
   * @return true if the current text contains an applicable replacement source.
   */
  bool HasInlineReplacementSource() const;

  /**
   * @brief Removes on-demand inline replacement data and disconnects resource notifications.
   */
  void ClearInlineReplacementData();

  /**
   * @brief Updates on-demand inline replacement data from the final text layout.
   *
   * @param[in] updateData The authored replacement source and final placements.
   * @param[in] ownerSize The control size.
   * @param[in] padding The effective text padding.
   */
  void UpdateInlineReplacementData(const InlineReplacementUpdateData& updateData, const Vector2& ownerSize, const Insets& padding);

  /**
   * @brief Refreshes inline replacement visuals after resource loading.
   *
   * @param[in] view The visual owner.
   */
  void OnInlineReplacementResourcesReady(Ui::View view);

  /**
   * @brief Stops the current sync marquee so it can be rebuilt on the next relayout.
   */
  void RequestSyncMarqueeRestart();

  /**
   * @brief Updates the TextVisual with the current TextGradient snapshot.
   */
  void UpdateTextGradientStyle();

  /**
   * @brief Updates the TextVisual with the current TextGradientOverlay snapshot.
   */
  void UpdateTextGradientOverlayStyle();

  /**
   * @brief Updates hidden TextGradient animation source properties from the authored TextGradient.
   */
  void SyncGradientAnimProperties();

  /**
   * @brief Updates hidden TextGradientOverlay animation source properties from the authored TextGradientOverlay.
   */
  void SyncGradientOverlayAnimProperties();

  /**
   * @brief Returns true when the current TextGradient can drive the supported animation uniforms.
   */
  bool IsGradientAnimSupported() const;

  /**
   * @brief Returns true when the current TextGradientOverlay can drive the supported animation uniforms.
   *
   * This checks the gradient data only. Renderer/scroller binding happens separately.
   */
  bool IsGradientOverlayAnimSupported() const;

  /**
   * @brief Binds current TextGradient animation source property indices to the TextVisual.
   */
  void BindGradientAnimProperties();

  /**
   * @brief Binds current TextGradientOverlay animation source property indices to the TextVisual.
   *
   * Also updates an active TextScroller when the overlay source property is
   * created after full marquee setup.
   */
  void BindGradientOverlayAnimProperties();

  /**
   * @brief Updates TextGradient animation constraint apply rate on the visual and active scroller.
   *
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientAnimApplyRate(bool notifyToConstraint = false);

  /**
   * @brief Updates TextGradientOverlay animation constraint apply rate on the visual and active scroller.
   *
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientOverlayAnimApplyRate(bool notifyToConstraint = false);

  /**
   * @brief Updates the effective line height based on the current LineHeightMode.
   */
  void UpdateLineHeight();

  /**
   * @brief Clears text fit state and invalidates text measurement if needed.
   */
  void ClearTextFitInternal();

  /**
   * @brief Callback function for when the layout is changed.
   * @param[in] actor The actor whose layoutDirection is changed.
   * @param[in] type  The layoutDirection.
   */
  void OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type);

  /**
   * @brief Callback function for when the locale is changed.
   * @param[in] locale The new system locale.
   */
  void OnLocaleChanged(std::string locale);

  /**
   * @brief Handles touch interactions for text anchors.
   *
   * @param[in] actor The label that received the touch.
   * @param[in] touch The touch event information.
   * @return True if the touch event is consumed, otherwise false.
   */
  bool OnInterceptTouched(Actor actor, TouchEvent touch);

  /**
   * @brief Updates touch interception based on anchor presence.
   */
  void UpdateAnchorTouchInterception();

  /**
   * @brief Clears the stored StyledText source state.
   */
  void ClearStyledTextSourceState();

  /**
   * @brief Clears async anchor interaction state.
   */
  void ClearAnchorInteractionState();

  /**
   * @brief Gets the text content offset in label coordinates.
   */
  Vector2 GetTextContentOffset() const;

  /**
   * @brief Clears accessibility anchor actors.
   */
  void ClearA11yAnchors();

  /**
   * @brief Updates accessibility anchor actors.
   */
  void UpdateA11yAnchors(bool contentDirty);

  /**
   * @brief Updates sync accessibility anchor actors from the controller.
   */
  void UpdateSyncA11yAnchors();

  /**
   * @brief Updates async accessibility anchor actors from cached hit regions.
   */
  void UpdateAsyncA11yAnchors();

  /**
   * @brief Called when accessibility is enabled or disabled.
   */
  void OnAccessibilityStatusChanged();

  /**
   * @brief Initializes marquee based on the current text layout.
   *
   * @param[in] contentSize The size of the text content excluding padding.
   * @param[in] originSize The size of the laid-out text used to compute the initial scroll offset.
   */
  void InitializeMarquee(const Size& contentSize, const Size& originSize);

  /**
   * @brief Updates the marquee state.
   *
   * Called when properties such as orientation or multiline state change.
   */
  void UpdateMarqueeState();

  /**
   * @brief Enables automatic marquee evaluation.
   *
   * Clears the suppression state so that marquee can be re-evaluated
   * based on the current layout conditions (e.g., overflow, visibility).
   */
  void EnableAutoMarqueeEvaluation();

  /**
   * @brief Suppresses automatic marquee evaluation.
   *
   * Prevents marquee from being automatically enabled even if
   * layout conditions (e.g., overflow) are satisfied.
   */
  void SuppressAutoMarqueeEvaluation();

  /**
   * @brief Stops the current marquee animation immediately, preserving the configured stop mode.
   */
  void StopMarqueeImmediately();

  /**
   * @brief Handles marquee behavior when visibility changes.
   *
   * @param[in] visible True to restore the previous marquee state,
   *                    false to stop the marquee animation.
   */
  void OnMarqueeVisibilityChanged(bool visible);

  /**
   * @brief Returns the text scroller, creating it if necessary.
   *
   * @return The text scroller.
   */
  Ui::Text::TextScrollerPtr GetTextScroller();

  /**
   * @brief Invalidates static geometry retained for a later marquee transition.
   */
  void InvalidateMarqueeStartGeometry();

  /**
   * @brief Captures current synchronous static geometry before marquee layout.
   *
   * Async rendering publishes the equivalent descriptors through its render result.
   */
  void CaptureMarqueeStartGeometry();

  /**
   * @brief Enables or disables the marquee animation.
   *
   * Updates the internal marquee state and starts or stops the scrolling accordingly.
   *
   * @param[in] enabled True to start the marquee animation, false to stop it.
   */
  void SetMarqueeEnabled(bool enabled);

  /**
   * @brief Callback when the visibility of the actor is changed.
   */
  void OnViewEffectiveVisibilityChanged(Actor actor, bool visible);

  /**
   * @brief Returns the cached effective visibility of the label.
   *
   * @return True if the label is effectively visible.
   */
  bool IsVisible();

  /**
   * @brief Evaluates whether marquee should run and applies the result.
   *
   * Enables or disables marquee depending on the current layout and mode.
   *
   * @param[in] contentSize The size of the text content excluding padding.
   * @param[in] orientation The marquee orientation.
   */
  void EvaluateAndApplyMarquee(const Size& contentSize, Ui::Text::MarqueeOrientation orientation);

  /**
   * @brief Prepares layout data required for marquee.
   *
   * @param[in] contentSize The size of the text content excluding padding.
   * @param[in] orientation The marquee orientation.
   * @param[out] originSize The laid-out text size used to compute the initial scroll offset.
   */
  void PrepareMarqueeLayout(const Size& contentSize, Ui::Text::MarqueeOrientation orientation, Size& originSize);

  /**
   * @brief Called when a font variation property changes.
   */
  void OnVariationPropertyNotify(PropertyNotification source);

  /**
   * @brief Handles font variation property updates.
   *
   * @param[in] index The property index being set.
   * @param[in] propertyValue The property value.
   * @return true if handled, false otherwise.
   */
  bool HandleVariationPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue);

  /**
   * @brief Applies the cutout enabled state internally.
   *
   * This updates the internal rendering state and synchronizes any
   * dependent background or visual configuration required for cutout rendering.
   *
   * @param[in] enabled True to enable cutout rendering, false otherwise.
   */
  void SetCutoutEnabledInternal(bool enabled);

  /**
   * @brief Sets whether the background of the view is enabled.
   *
   * @param[in] enabled True to enable the view background, false to disable it.
   */
  void SetViewBackgroundEnabled(bool enabled);

  /**
   * @brief Gets the current background color of the view.
   *
   * Extracts the color from the view's background visual if available.
   *
   * @param[out] backgroundColor The extracted background color.
   * @return True if a background color was successfully retrieved, false otherwise.
   */
  bool GetViewBackgroundColor(Vector4& backgroundColor) const;

  /**
   * @brief Handles background property changes related to cutout rendering.
   */
  void OnBackgroundPropertyChanged();

  /**
   * @brief Updates dependent rendering state for cutout rendering.
   *
   * This synchronizes background and visual state after the cutout property
   * has been applied through the property handler.
   *
   * @param[in] enabled True if cutout rendering is enabled, false otherwise.
   */
  void UpdateCutoutState(bool enabled);

  /**
   * @brief Applies a localized text string to the label.
   *
   * This is used as the UiLocalizationManager binding callback for the
   * translatable text binding. The target argument is ignored because the
   * callback is bound to this LabelImpl instance.
   *
   * @param[in] target The target object for the binding.
   * @param[in] text The localized text to display.
   */
  void ApplyLocalizedText(BaseHandle target, const Dali::String& text);

  /**
   * @brief Returns the parameters used for asynchronous text processing.
   *
   * Collects and returns the text label properties required to perform
   * asynchronous text rendering or size computation.
   *
   * @param[in] requestType The type of asynchronous request.
   * @param[in] contentSize The content size requested by relayout, excluding padding.
   * @param[in] padding The label padding.
   * @param[in] layoutDirection The layout direction.
   * @return The parameters for asynchronous text processing.
   */
  Ui::Text::AsyncTextParameters GetAsyncTextParameters(Text::Async::RequestType    requestType,
                                                       const Vector2&              contentSize,
                                                       const Insets&               padding,
                                                       Dali::LayoutDirection::Type layoutDirection);

  /**
   * @brief Emits TextFitChanged signal.
   */
  void EmitTextFitChanged();

  /**
   * @brief Emits AsyncRenderFinished signal.
   */
  void EmitAsyncRenderFinished(float width, float height);

  /**
   * @brief Emits AsyncNaturalSizeComputed signal.
   */
  void EmitAsyncNaturalSizeComputed(float width, float height);

  /**
   * @brief Emits AsyncHeightForWidthComputed signal.
   */
  void EmitAsyncHeightForWidthComputed(float width, float height);

  // Properties
public:
  /**
   * @copydoc View::OnPropertySet()
   */
  void OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue) override;

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Dali::Property::Value GetProperty(BaseObject* object, Dali::Property::Index index);

private: // UiColorManager
  void SetTextColorInternal(const Vector4& color);
  void SetAnchorColorInternal(const Vector4& color);
  void SetAnchorClickedColorInternal(const Vector4& color);
  void SetTextBackgroundColorInternal(const Vector4& color);
  void SetUnderlineColorInternal(const Vector4& color);
  void SetShadowColorInternal(const Vector4& color);
  void SetOutlineColorInternal(const Vector4& color);
  void SetLineThroughColorInternal(const Vector4& color);
  void SetBevelLightColorInternal(const Vector4& color);
  void SetBevelShadowColorInternal(const Vector4& color);

private:
  // Not copyable or movable
  LabelImpl(const LabelImpl&)            = delete;
  LabelImpl(LabelImpl&&)                 = delete;
  LabelImpl& operator=(const LabelImpl&) = delete;
  LabelImpl& operator=(LabelImpl&&)      = delete;

private:
  // Data
  Signal<void(View, const Dali::String&)> mAnchorClickedSignal;
  Signal<void(View, float, float)>        mAsyncRenderFinishedSignal;
  Signal<void(View, float, float)>        mAsyncNaturalSizeComputedSignal;
  Signal<void(View, float, float)>        mAsyncHeightForWidthComputedSignal;

  Internal::Text::TextGradientPropertyDataPtr mTextGradientPropertyData;
  Internal::Text::StyledTextSourceDataPtr     mStyledTextSourceData;
  Internal::Text::TextRevealDataPtr           mTextRevealData;

  Visual::Base              mVisual;
  Ui::Text::ControllerPtr   mController;
  Ui::Text::TextScrollerPtr mTextScroller;

  Vector2                               mSize;
  Vector2                               mLastMeasureConstraints;
  Vector2                               mLastMeasureRequestedSize;
  float                                 mLineHeight;
  Ui::Text::LineHeightMode              mLineHeightMode;
  Ui::Text::OverflowMode                mOverflowMode;
  Ui::Text::MarqueeTriggerPolicy        mMarqueeTriggerPolicy;
  Ui::Text::MarqueeStartAnchor          mMarqueeStartAnchor;
  Ui::Text::MarqueeFittingStartGeometry mMarqueeFittingStartGeometry;

  int  mAsyncLineCount;
  int  mTextColorAnimatedCount;
  bool mRendererUpdateNeeded : 1;     // Whether the text renderer needs to be updated.
  bool mMeasureInvalidated : 1;       // whether measurement has been invalidated.
  bool mIsAsyncRenderRequested : 1;   // whether an async render has been requested.
  bool mIsContentLayoutDirty : 1;     // Whether content size or padding has changed.
  bool mSuppressAutoMarquee : 1;      // whether automatic marquee evaluation is suppressed.
  bool mLastMarqueeEnabled : 1;       // whether manual marquee was enabled in the previous state.
  bool mRestartMarquee : 1;           // whether sync marquee needs one-shot restart after measure-affecting changes.
  bool mHasLastMeasureMetrics : 1;    // whether the last OnMeasure inputs have been captured.
  bool mHasStyledTextSource : 1;      // whether current text source was set by SetStyledText().
  bool mHasVariationProperties : 1;   // whether font variation custom properties have been registered.
  bool mHasAnchors : 1;               // whether the text has anchors.
  bool mHasAsyncAnchorHitRegions : 1; // whether active async anchor hit regions are cached.
  bool mAsyncAnchorGeometryDirty : 1; // whether cached async anchor geometry may no longer match the content layout.
  bool mHasA11yAnchors : 1;           // whether accessibility TextAnchor actors are attached.
  bool mIsVisible : 1;                // cached result of IsOnSceneVisible().
  bool mIsVisibleInitialized : 1;     // whether mIsVisible has been initialized.
  bool mIsViewBackgroundEnabled : 1;  // whether the view background is enabled.
  bool mIsManualRenderInProgress : 1; // True while an async manual render request is in progress. Reset to false when completed.
  bool mIsManualRenderFinished : 1;   // True when an async manual render has completed. Reset to false on the next relayout.

protected:
  struct PropertyHandler;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
