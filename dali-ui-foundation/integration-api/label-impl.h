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
class DALI_UI_API LabelImpl : public ViewImpl, public Text::ControlInterface
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
  void SetText(const std::string text);

  /**
   * @copydoc Dali::Ui::Label::GetText
   */
  std::string GetText() const;

  /**
   * @copydoc Dali::Ui::Label::SetFontFamily
   */
  void SetFontFamily(std::string fontFamily);

  /**
   * @copydoc Dali::Ui::Label::GetFontFamily
   */
  std::string GetFontFamily() const;

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
  void SetTextColor(const Vector4& color);

  /**
   * @copydoc Dali::Ui::Label::GetTextColor
   */
  const Vector4& GetTextColor() const;

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

private:
  // Not copyable or movable
  LabelImpl(const LabelImpl&)            = delete;
  LabelImpl(LabelImpl&&)                 = delete;
  LabelImpl& operator=(const LabelImpl&) = delete;
  LabelImpl& operator=(LabelImpl&&)      = delete;

private:
  // Data
  Visual::Base        mVisual;
  Text::ControllerPtr mController;

  int  mTextColorAnimatedCount;
  bool mTextUpdateNeeded : 1;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
