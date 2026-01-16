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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace UI
{

namespace Internal
{

/**
 * @brief This is the internal implementation class for View.
 *
 * ViewImpl extends ControlImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the View public API.
 *
 * @see Dali::UI::View
 */
class View : public Toolkit::Internal::Control
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new View.
   */
  static UI::View New();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~View();

public:

  // API

protected:

  // Construction

  /**
   * @brief View constructor.
   *
   */
  View();

public:

  // From Toolkit::Internal::Control

  /**
   * @copydoc Toolkit::Internal::Control::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Toolkit::Internal::Control::OnKeyEvent
   */
  bool OnKeyEvent(const KeyEvent& event) override;

public:
  /**
   * @copydoc Dali::UI::View::GetSizeWidth
   */
  float GetSizeWidth() const;

  /**
   * @copydoc Dali::UI::View::SetSizeWidth
   */
  void SetSizeWidth(float width);

  /**
   * @copydoc Dali::UI::View::GetSizeHeight
   */
  float GetSizeHeight() const;

  /**
   * @copydoc Dali::UI::View::SetSizeHeight
   */
  void SetSizeHeight(float height);

  /**
   * @copydoc Dali::UI::View::GetPositionX
   */
  float GetPositionX() const;

  /**
   * @copydoc Dali::UI::View::SetPositionX
   */
  void SetPositionX(float x);

  /**
   * @copydoc Dali::UI::View::GetPositionY
   */
  float GetPositionY() const;

  /**
   * @copydoc Dali::UI::View::SetPositionY
   */
  void SetPositionY(float y);

  /**
   * @copydoc Dali::UI::View::GetParentOrigin
   */
  Vector3 GetParentOrigin() const;

  /**
   * @copydoc Dali::UI::View::SetParentOrigin
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @copydoc Dali::UI::View::GetPivotPoint
   */
  Vector3 GetPivotPoint() const;

  /**
   * @copydoc Dali::UI::View::SetPivotPoint
   */
  void SetPivotPoint(const Vector3& point);

private:

  // Not copyable or movable
  View(const View&) = delete;
  View(View&&) = delete;
  View& operator=(const View&) = delete;
  View& operator=(View&&) = delete;

private:

  // Data
};

// Helpers for public-api forwarding methods

inline Internal::View& GetImpl(UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Internal::View&>(handle);
}

inline const Internal::View& GetImpl(const UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Internal::View&>(handle);
}

} // namespace Internal

} // namespace UI

} // namespace Dali
