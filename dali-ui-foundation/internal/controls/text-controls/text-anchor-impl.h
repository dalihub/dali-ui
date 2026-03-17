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
#include <dali/devel-api/atspi-interfaces/hyperlink.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>
#include <dali-ui-foundation/internal/text/text-anchor-control-interface.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class TextAnchorImpl;
using TextAnchorImplPtr = IntrusivePtr<TextAnchorImpl>;

/**
 * @brief A control which renders anchor (hyperlink) in hypertext.
 */
class TextAnchorImpl : public Integration::ViewImpl
{
public:
  /**
   * @brief Creates a new TextAnchor.
   */
  static TextAnchorImplPtr New();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextAnchorImpl();

  // Properties

public:
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

private: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Ui::Internal::Control::CreateAccessibleObject()
   */
  ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc Control::OnAccessibilityActivated()
   */
  bool OnAccessibilityActivated() override;

private: // Implementation
  /**
   * Construct a new TextAnchorImpl.
   */
  TextAnchorImpl();

private:
  // Undefined copy constructor and assignment operators
  TextAnchorImpl(const TextAnchorImpl&);
  TextAnchorImpl& operator=(const TextAnchorImpl& rhs);

  // Data
  int         mStartCharacterIndex;
  int         mEndCharacterIndex;
  std::string mUri;

protected:
  /**
   * @brief This structure is to connect TextAnchor with Accessible functions.
   */
  class TextAnchorAccessible : public ControlAccessible, public Dali::Accessibility::Hyperlink
  {
  public:
    using ControlAccessible::ControlAccessible;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetEndIndex()
     */
    int32_t GetEndIndex() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetStartIndex()
     */
    int32_t GetStartIndex() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorCount()
     */
    int32_t GetAnchorCount() const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorAccessible()
     */
    Accessible* GetAnchorAccessible(int32_t anchorIndex) const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::GetAnchorUri()
     */
    std::string GetAnchorUri(int32_t anchorIndex) const override;

    /**
     * @copydoc Dali::Accessibility::Hyperlink::IsValid()
     */
    bool IsValid() const override;

  protected:
    /**
     * @copydoc Dali::Accessibility::Accessible::InitDefaultFeatures()
     */
    void InitDefaultFeatures() override;
  };
};

inline Ui::Internal::TextAnchorImpl& GetImpl(Ui::TextAnchor& textAnchor)
{
  DALI_ASSERT_ALWAYS(textAnchor);

  Dali::RefObject& handle = textAnchor.GetImplementation();

  return static_cast<Ui::Internal::TextAnchorImpl&>(handle);
}

inline const Ui::Internal::TextAnchorImpl& GetImpl(const Ui::TextAnchor& textAnchor)
{
  DALI_ASSERT_ALWAYS(textAnchor);

  const Dali::RefObject& handle = textAnchor.GetImplementation();

  return static_cast<const Ui::Internal::TextAnchorImpl&>(handle);
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
