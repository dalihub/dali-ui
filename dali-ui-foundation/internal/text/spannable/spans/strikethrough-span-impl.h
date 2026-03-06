#ifndef DALI_UI_INTERNAL_TEXT_STRIKETHROUGH_SPAN_IMPL_H
#define DALI_UI_INTERNAL_TEXT_STRIKETHROUGH_SPAN_IMPL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/devel-api/text/spans/strikethrough-span.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/spannable/spans/base-span-impl.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
class StrikethroughSpan;

using StrikethroughSpanPtr = IntrusivePtr<StrikethroughSpan>;

/**
 * @copydoc Dali::Ui::Text::StrikethroughSpan
 */

class StrikethroughSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new StrikethroughSpan object.
   *
   * @return A new StrikethroughSpan object.
   */
  static Dali::Ui::Text::StrikethroughSpan New();

  /**
   * @brief Creates a new StrikethroughSpan object.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A new StrikethroughSpan object.
   */
  static Dali::Ui::Text::StrikethroughSpan New(Vector4 color, float height);

  /**
   * Default Constructor
   */
  StrikethroughSpan();
  StrikethroughSpan(const StrikethroughSpan&) = delete;            ///< Deleted copy constructor
  StrikethroughSpan(StrikethroughSpan&&) = delete;                 ///< Deleted move constructor
  StrikethroughSpan& operator=(const StrikethroughSpan&) = delete; ///< Deleted copy assignment operator
  StrikethroughSpan& operator=(StrikethroughSpan&&) = delete;      ///< Deleted move assignment operator

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~StrikethroughSpan() override;

public: // Methods
  /**
   * @copydoc Dali::Ui::Text::StrikethroughSpan::GetColor()
   */
  Vector4 GetColor() const;

  /**
   * @copydoc Dali::Ui::Text::StrikethroughSpan::IsColorDefined()
   */
  bool IsColorDefined() const;

  /**
   * @copydoc Dali::Ui::Text::StrikethroughSpan::GetHeight()
   */
  float GetHeight() const;

  /**
   * @copydoc Dali::Ui::Text::StrikethroughSpan::IsHeightDefined()
   */
  bool IsHeightDefined() const;

public: // Methods. Not intended for application developers
  /**
   * @brief Set the color of underline.
   *
   * @param[in] color The color of line.
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Set the height of underline.
   *
   * @param[in] height The height of line.
   */
  void SetHeight(const float& height);

public: // Methods for internal only
  /**
   * @copydoc Dali::Ui::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel,
                               const Dali::Ui::Text::Range& range) const override;

private:
  struct Impl;

  std::unique_ptr<Impl> mImpl{nullptr};

}; // class StrikethroughSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::StrikethroughSpan& GetImplementation(Dali::Ui::Text::StrikethroughSpan& foregroundColorSpan)
{
  DALI_ASSERT_ALWAYS(foregroundColorSpan && "foregroundColorSpan handle is empty");
  BaseObject& object = foregroundColorSpan.GetBaseObject();
  return static_cast<Internal::StrikethroughSpan&>(object);
}

inline const Internal::StrikethroughSpan& GetImplementation(
    const Dali::Ui::Text::StrikethroughSpan& foregroundColorSpan)
{
  DALI_ASSERT_ALWAYS(foregroundColorSpan && "foregroundColorSpan handle is empty");
  const BaseObject& object = foregroundColorSpan.GetBaseObject();
  return static_cast<const Internal::StrikethroughSpan&>(object);
}

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_STRIKETHROUGH_SPAN_IMPL_H