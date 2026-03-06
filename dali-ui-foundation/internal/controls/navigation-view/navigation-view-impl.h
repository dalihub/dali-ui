#ifndef DALI_UI_INTERNAL_NAVIGATION_CONTROL_H
#define DALI_UI_INTERNAL_NAVIGATION_CONTROL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/layer.h>
#include <list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/navigation-view/navigation-view.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>

namespace Dali
{
namespace Ui
{
class NavigationView;

namespace Internal
{
class NavigationBar;

/**
 * @brief
 *
 * NavigationView implements a controller than manages the navigation of hierarchical contents.
 */

class NavigationView : public Control
{
public:
  /**
   * Create an initialized NavigationView.
   * @return A handle to a newly allocated Dali resource
   */
  static Ui::NavigationView New();

  /**
   * @copydoc Dali::Ui::NavigationView::Push()
   */
  void Push(Actor& actor);

  /**
   * @copydoc Dali::Ui::NavigationView::Pop()
   */
  Actor Pop();

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] properties The properties with which to perform this action.
   * @return true if action has been accepted by this control
   */
  // static bool DoAction( BaseObject* object, const std::string& actionName, const Property::Map& properties );

private: // override functions from Control
  /**
   * @copydoc Constrol::OnInitialize
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Control::OnSceneConnection( int depth  )
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Control::OnRelayout()
   */
  // virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

protected:
  /**
   * Constructor.
   * It initializes the NavigationView members
   */
  NavigationView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~NavigationView();

private:
  // Undefined
  NavigationView(const NavigationView&);

  // Undefined
  NavigationView& operator=(const NavigationView& rhs);

private:
  std::vector<Actor> mContentStack;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Ui::Internal::NavigationView& GetImpl(Ui::NavigationView& navigationView)
{
  DALI_ASSERT_ALWAYS(navigationView);

  Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<Ui::Internal::NavigationView&>(handle);
}

inline const Ui::Internal::NavigationView& GetImpl(const Ui::NavigationView& navigationView)
{
  DALI_ASSERT_ALWAYS(navigationView);

  const Dali::RefObject& handle = navigationView.GetImplementation();

  return static_cast<const Ui::Internal::NavigationView&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_NAVIGATION_CONTROL_H
