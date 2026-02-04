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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/integration-api/trait-id.h>

namespace Dali
{

namespace UI
{

namespace Integration
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
class DALI_UI_API ViewImpl : public Toolkit::Internal::Control
{
public:

  // Creation & Destruction

  /**
   * @brief Creates a new View.
   */
  static UI::View New();

protected:

  /**
   * @brief Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ViewImpl();

public:

  // API

protected:

  // Construction

  /**
   * @brief View constructor.
   */
  ViewImpl();

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

  /**
   * @brief Sets a trait to this View.
   *
   * The trait will share the lifecycle with this View.
   *
   * @throws DaliException If the trait already has an owner
   * @throws DaliException If the id is already registered with other trait
   *
   * @note **Strong Reference**
   * The view will hold the strong reference to the trait after attached.
   *
   * @param[in] id The unique key to identify the trait
   * @param[in] trait The trait object to attach
   */
  void SetTrait(TraitId id, Trait& trait);

  /**
   * @brief Gets a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return The trait handle
   */
  Trait GetTrait(TraitId id) const;

  /**
   * @brief Removes a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return True if succeeded, false otherwise
   */
  bool RemoveTrait(TraitId id);

private:

  // Not copyable or movable
  ViewImpl(const ViewImpl&) = delete;
  ViewImpl(ViewImpl&&) = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&) = delete;

private:

  std::vector<std::pair<TraitId, Trait>> mTraits;
};

// Helpers for public-api forwarding methods

inline Integration::ViewImpl& GetImpl(UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Integration::ViewImpl&>(handle);
}

inline const Integration::ViewImpl& GetImpl(const UI::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Integration::ViewImpl&>(handle);
}

} // namespace Integration

} // namespace UI

} // namespace Dali
