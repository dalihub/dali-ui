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
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/text-control-interface.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>

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
class DALI_UI_API LabelImpl : public ViewImpl, public Text::ControlInterface, public Text::AnchorControlInterface
{
public:
  /**
   * @brief Temporary animatable property definitions for LabelImpl.
   * @details
   * TODO: Until the official animatable property support interface is finalized,
   * animatable properties are registered and handled directly in LabelImpl.
   * This structure may be refactored once the unified animatable property
   * interface is decided.
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the LabelImpl class.
     */
    enum
    {
      ///////////////////////////////////////////////////////////////////////////////
      // Animatable Properties
      ///////////////////////////////////////////////////////////////////////////////

      /**
       * @brief The color of the text.
       * @details Name "textColor", type Property::VECTOR4.
       */
      TEXT_COLOR = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,

      /**
       * @brief The red component of the text color.
       * @details Name "textColorRed", type Property::FLOAT.
       * @see TEXT_COLOR
       */
      TEXT_COLOR_RED,

      /**
       * @brief The green component of the text color.
       * @details Name "textColorGreen", type Property::FLOAT.
       * @see TEXT_COLOR
       */
      TEXT_COLOR_GREEN,

      /**
       * @brief The blue component of the text color.
       * @details Name "textColorBlue", type Property::FLOAT.
       * @see TEXT_COLOR
       */
      TEXT_COLOR_BLUE,

      /**
       * @brief The alpha component of the text color.
       * @details Name "textColorAlpha", type Property::FLOAT.
       * @see TEXT_COLOR
       */
      TEXT_COLOR_ALPHA
    };
  };

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

public: // Signals
  /**
   * @copydoc Dali::Ui::Label::AnchorClickedSignal()
   */
  Signal<void(View, const Dali::String&)>& AnchorClickedSignal();

protected:
  // Construction

  /**
   * @brief LabelImpl constructor.
   */
  LabelImpl();

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

public: // From Control
        // TODO: If the implementation in Control is moved to ViewImpl, this part will need to be updated accordingly.
  /**
   * @copydoc Control::OnAnimateAnimatableProperty()
   */
  void OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index,
                                   Dali::Animation::State state) override;

  /**
   * @copydoc Control::OnConstraintAnimatableProperty()
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied) override;

protected: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc Integration::ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

public: // From ControlInterface
  /**
   * @copydoc Text::ControlInterface::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

public: // From AnchorControlInterface
  /**
   * @copydoc Text::AnchorControlInterface::AnchorClicked()
   */
  bool AnchorClicked(uint32_t cursorPosition, std::string& href) override;

  /**
   * @copydoc Text::AnchorControlInterface::EmitAnchorClickedSignal()
   */
  void EmitAnchorClickedSignal(const std::string& href) override;

private: // Implementation
  /**
   * @brief Sets the minimum line height used by the text controller.
   */
  void SetMinimumLineHeight(float height);

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

private: // UiColorManager
  void SetTextColorInternal(const Vector4& color);
  void SetAnchorColorInternal(const Vector4& color);
  void SetAnchorClickedColorInternal(const Vector4& color);

private:
  // Not copyable or movable
  LabelImpl(const LabelImpl&)            = delete;
  LabelImpl(LabelImpl&&)                 = delete;
  LabelImpl& operator=(const LabelImpl&) = delete;
  LabelImpl& operator=(LabelImpl&&)      = delete;

private:
  // Data
  Signal<void(View, const Dali::String&)> mAnchorClickedSignal;

  Visual::Base        mVisual;
  Text::ControllerPtr mController;

  Vector2              mTouchPosition; ///< The initial touch down position.
  float                mLineHeight;
  Text::LineHeightMode mLineHeightMode;

  int  mTextColorAnimatedCount;
  bool mTextUpdateNeeded : 1;
  bool mIsTouchDown : 1; // whether the currently intercepted touch is in the down state.
  bool mHasAnchors : 1;  // whether the text has anchors or not.
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
