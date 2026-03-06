#ifndef DALI_UI_INTERNAL_TEXT_FOREGROUND_COLOR_SPAN_IMPL_H
#define DALI_UI_INTERNAL_TEXT_FOREGROUND_COLOR_SPAN_IMPL_H

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
#include <dali-ui-foundation/devel-api/text/spans/foreground-color-span.h>
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
class ForegroundColorSpan;
using ForegroundColorSpanPtr = IntrusivePtr<ForegroundColorSpan>;

/**
 * @copydoc Dali::Ui::Text::ForegroundColorSpan
 */
class ForegroundColorSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new ForegroundColorSpan object.
   */
  static Dali::Ui::Text::ForegroundColorSpan New(Vector4 color);

  /**
   * Default Constructor
   */
  ForegroundColorSpan();

  ForegroundColorSpan(const ForegroundColorSpan&) = delete;            ///< Deleted copy constructor
  ForegroundColorSpan(ForegroundColorSpan&&) = delete;                 ///< Deleted move constructor
  ForegroundColorSpan& operator=(const ForegroundColorSpan&) = delete; ///< Deleted copy assignment operator
  ForegroundColorSpan& operator=(ForegroundColorSpan&&) = delete;      ///< Deleted move assignment operator

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~ForegroundColorSpan() override;

public: // Methods
  /**
   * @copydoc Dali::Ui::Text::ForegroundColorSpan::GetForegroundColor()
   */
  Vector4 GetForegroundColor() const;

  /**
   * @copydoc Dali::Ui::Text::ForegroundColorSpan::IsForegroundColorDefined()
   */
  bool IsForegroundColorDefined() const;

public: // Methods. Not intended for application developers
  /**
   * @brief Set the foreground-color.
   *
   * @param[in] color The foreground-color.
   */
  void SetForegroundColor(Vector4 color);

public: // Methods for internal only
  /**
   * @copydoc Dali::Ui::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel,
                               const Dali::Ui::Text::Range& range) const override;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class ForegroundColorSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ForegroundColorSpan& GetImplementation(Dali::Ui::Text::ForegroundColorSpan& foregroundColorSpan)
{
  DALI_ASSERT_ALWAYS(foregroundColorSpan && "foregroundColorSpan handle is empty");

  BaseObject& object = foregroundColorSpan.GetBaseObject();

  return static_cast<Internal::ForegroundColorSpan&>(object);
}

inline const Internal::ForegroundColorSpan& GetImplementation(
    const Dali::Ui::Text::ForegroundColorSpan& foregroundColorSpan)
{
  DALI_ASSERT_ALWAYS(foregroundColorSpan && "foregroundColorSpan handle is empty");

  const BaseObject& object = foregroundColorSpan.GetBaseObject();

  return static_cast<const Internal::ForegroundColorSpan&>(object);
}

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_TEXT_FOREGROUND_COLOR_SPAN_IMPL_H