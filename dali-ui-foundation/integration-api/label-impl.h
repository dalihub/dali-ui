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
#include <dali-ui-foundation/integration-api/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/text-control-interface.h>
#include <dali-ui-foundation/internal/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/text/text-scroller.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit-range.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>

#include <unordered_map>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class LabelImpl;
using LabelImplPtr = IntrusivePtr<LabelImpl>;

/**
 * @brief This is the internal implementation class for Label.
 *
 * @see Dali::Ui::LabelImpl
 */
class DALI_UI_API LabelImpl : public ViewImpl, public Text::ControlInterface, public Text::ScrollerInterface, public Text::AnchorControlInterface, public Text::AsyncTextInterface
{
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
  void SetLineWrapMode(Text::LineWrapMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLineWrapMode
   */
  Text::LineWrapMode GetLineWrapMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetTextColor
   */
  void SetTextColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::Label::GetTextColor
   */
  UiColor GetTextColor();

  /**
   * @copydoc Dali::Ui::Label::SetHorizontalTextAlignment
   */
  void SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::Label::GetHorizontalTextAlignment
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::Label::SetVerticalTextAlignment
   */
  void SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @copydoc Dali::Ui::Label::GetVerticalTextAlignment
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @copydoc Dali::Ui::Label::SetOverflowMode
   */
  void SetOverflowMode(Text::OverflowMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetOverflowMode
   */
  Text::OverflowMode GetOverflowMode() const;

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
  void SetLineHeightMode(Text::LineHeightMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLineHeightMode
   */
  Text::LineHeightMode GetLineHeightMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetLayoutDirectionMode
   */
  void SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @copydoc Dali::Ui::Label::GetLayoutDirectionMode
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarkupEnabled
   */
  void SetMarkupEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::Label::IsMarkupEnabled
   */
  bool IsMarkupEnabled() const;

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
   * @copydoc Dali::Ui::Label::SetMarqueeStopMode
   */
  void SetMarqueeStopMode(Text::MarqueeStopMode stopMode);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeStopMode
   */
  Text::MarqueeStopMode GetMarqueeStopMode() const;

  /**
   * @copydoc Dali::Ui::Label::SetMarqueeOrientation
   */
  void SetMarqueeOrientation(Text::MarqueeOrientation orientation);

  /**
   * @copydoc Dali::Ui::Label::GetMarqueeOrientation
   */
  Text::MarqueeOrientation GetMarqueeOrientation() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontWeight
   */
  void SetFontWeight(Text::FontWeight weight);

  /**
   * @copydoc Dali::Ui::Label::GetFontWeight
   */
  Text::FontWeight GetFontWeight() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontWidth
   */
  void SetFontWidth(Text::FontWidth width);

  /**
   * @copydoc Dali::Ui::Label::GetFontWidth
   */
  Text::FontWidth GetFontWidth() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontSlant
   */
  void SetFontSlant(Text::FontSlant slant);

  /**
   * @copydoc Dali::Ui::Label::GetFontSlant
   */
  Text::FontSlant GetFontSlant() const;

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
   * @copydoc Dali::Ui::Label::SetUnderline
   */
  void SetUnderline(const Text::Underline& underline);

  /**
   * @copydoc Dali::Ui::Label::ClearUnderline
   */
  void ClearUnderline();

  /**
   * @copydoc Dali::Ui::Label::SetShadow
   */
  void SetShadow(const Text::Shadow& shadow);

  /**
   * @copydoc Dali::Ui::Label::ClearShadow
   */
  void ClearShadow();

  /**
   * @copydoc Dali::Ui::Label::SetOutline
   */
  void SetOutline(const Text::Outline& outline);

  /**
   * @copydoc Dali::Ui::Label::ClearOutline
   */
  void ClearOutline();

  /**
   * @copydoc Dali::Ui::Label::SetLineThrough
   */
  void SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @copydoc Dali::Ui::Label::ClearLineThrough
   */
  void ClearLineThrough();

  /**
   * @copydoc Dali::Ui::Label::SetBevel
   */
  void SetBevel(const Text::Bevel& bevel);

  /**
   * @copydoc Dali::Ui::Label::ClearBevel
   */
  void ClearBevel();

  /**
   * @see Dali::Ui::Label::SetTextFit
   */
  void SetTextFit(const Text::FitRange& range);

  /**
   * @see Dali::Ui::Label::SetTextFit(const Dali::Vector<Text::FitCandidate>&)
   */
  void SetTextFit(const Dali::Vector<Text::FitCandidate>& candidates);

  /**
   * @copydoc Dali::Ui::Label::ClearTextFit
   */
  void ClearTextFit();

  /**
   * @copydoc Dali::Ui::Label::SetFontSizeScale
   */
  void SetFontSizeScale(float scale);

  /**
   * @copydoc Dali::Ui::Label::GetFontSizeScale
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
  void SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes);

  /**
   * @see Dali::Ui::Label::SetFontVariation(const Dali::String&)
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @copydoc Dali::Ui::Label::GetFontVariation
   */
  Dali::Vector<Text::FontVariationAxis> GetFontVariation() const;

  /**
   * @copydoc Dali::Ui::Label::ClearFontVariation
   */
  void ClearFontVariation();

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
   * @copydoc Dali::Ui::Label::SetCutoutEnabled
   */
  void SetCutoutEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::Label::IsCutoutEnabled
   */
  bool IsCutoutEnabled() const;

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

public: // Config
  /**
   * @brief Applies default values from UiConfigManager if initialized.
   */
  void ApplyInitialConfig();

public: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Integration::ViewImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Integration::ViewImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Integration::ViewImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

protected: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc Integration::ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc Integration::ViewImpl::OnPaddingSet()
   */
  void OnPaddingSet(const Extents& padding) override;

public: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnAnimateAnimatableProperty()
   */
  void OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index,
                                   Dali::Animation::State state) override;

  /**
   * @copydoc CustomActorImpl::OnConstraintAnimatableProperty()
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied) override;

public: // From ControlInterface
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
  void RequestAsyncRender();

private: // from ScrollerInterface
  /**
   * @copydoc Text::ScrollerInterface::ScrollingFinished()
   */
  void ScrollingFinished() override;

public: // From AnchorControlInterface
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  bool AnchorClicked(uint32_t cursorPosition, std::string& href) override;

  /**
   * @copydoc Text::AnchorControlInterface::EmitAnchorClickedSignal()
   */
  void EmitAnchorClickedSignal(const std::string& href) override;

public: // From AsyncTextInterface
  /**
   * @copydoc Text::AsyncTextInterface::AsyncInitializeMarquee()
   */
  void AsyncInitializeMarquee(Text::AsyncTextRenderInfo renderInfo) override;

  /**
   * @copydoc Text::AsyncTextInterface::AsyncTextFitChanged()
   */
  void AsyncTextFitChanged(float pointSize) override;

  /**
   * @copydoc Text::AsyncTextInterface::AsyncRenderFinished()
   */
  void AsyncRenderFinished(Text::AsyncTextRenderInfo renderInfo);

  /**
   * @copydoc Text::AsyncTextInterface::AsyncSizeComputed()
   */
  void AsyncSizeComputed(Text::AsyncTextRenderInfo renderInfo);

private: // Implementation
  /**
   * @brief Marks that the text renderer needs to be updated on the next relayout.
   */
  void RequestRendererUpdate();

  /**
   * @brief Updates the effective line height based on the current LineHeightMode.
   */
  void UpdateLineHeight();

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
   * @brief Handles touch interactions for text anchors when mark-up is enabled.
   *
   * @param[in] actor The label that received the touch.
   * @param[in] touch The touch event information.
   * @return True if the touch event is consumed, otherwise false.
   */
  bool OnInterceptTouched(Actor actor, const TouchEvent& touch);

  /**
   * @brief Updates touch interception based on anchor presence.
   */
  void UpdateAnchorTouchInterception();

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
  Text::TextScrollerPtr GetTextScroller();

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
  void OnControlInheritedVisibilityChanged(Actor actor, bool visible);

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
  void EvaluateAndApplyMarquee(const Size& contentSize, Text::MarqueeOrientation orientation);

  /**
   * @brief Prepares layout data required for marquee.
   *
   * @param[in] contentSize The size of the text content excluding padding.
   * @param[in] orientation The marquee orientation.
   * @param[out] originSize The laid-out text size used to compute the initial scroll offset.
   */
  void PrepareMarqueeLayout(const Size& contentSize, Text::MarqueeOrientation orientation, Size& originSize);

  /**
   * @brief Called when a font variation property changes.
   */
  void OnVariationPropertyNotify(PropertyNotification& source);

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
  Text::AsyncTextParameters GetAsyncTextParameters(Text::Async::RequestType requestType, const Vector2& contentSize, const Extents& padding, Dali::LayoutDirection::Type layoutDirection);

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

  std::unordered_map<Dali::Property::Index, Dali::String> mVariationIndexMap;
  WeakHandle<Ui::View>                                    mMaskSourceView;

  Visual::Base          mVisual;
  Text::ControllerPtr   mController;
  Text::TextScrollerPtr mTextScroller;

  Vector2              mTouchPosition; ///< The initial touch down position.
  float                mLineHeight;
  Text::LineHeightMode mLineHeightMode;
  Text::OverflowMode   mOverflowMode;

  int  mAsyncLineCount;
  int  mTextColorAnimatedCount;
  bool mRendererUpdateNeeded : 1;     // Whether the text renderer needs to be updated.
  bool mMeasureInvalidated : 1;       // whether measurement has been invalidated.
  bool mIsAsyncRenderRequested : 1;   // whether an async render has been requested.
  bool mIsSizeChanged : 1;            // whether the size has changed.
  bool mLastMarqueeEnabled : 1;       // whether marquee was enabled in the previous state.
  bool mIsTouchDown : 1;              // whether the currently intercepted touch is in the down state.
  bool mHasAnchors : 1;               // whether the text has anchors.
  bool mIsVisible : 1;                // cached result of IsEffectivelyVisible().
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
