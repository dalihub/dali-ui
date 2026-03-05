#ifndef DALI_UI_INTERNAL_SCROLL_VIEW_EFFECT_H
#define DALI_UI_INTERNAL_SCROLL_VIEW_EFFECT_H

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
#include <dali-ui-foundation/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{
class Animation;

namespace UI
{
class ScrollView;

namespace Internal
{
class ScrollViewEffect;

/**
 * @copydoc UI::ScrollViewEffect
 */
class ScrollViewEffect : public Dali::BaseObject, public ConnectionTracker
{
public:
  ScrollViewEffect();

  /**
   * Attaches this effect to scrollView.
   * @pre must not be already attached to a scrollView
   * @note internally the scrollView effect holds a weak reference
   * to scrollView.
   * @param[in] scrollView The scrollView instance to attach to.
   */
  void Attach(UI::ScrollView& scrollView);

  /**
   * Attaches this effect to scrollView.
   * @pre must not be already attached to a scrollView
   * @param[in] scrollView The scrollView instance to attach to.
   */
  void Detach(UI::ScrollView& scrollView);

public:
  /**
   * Called upon Attaching of effect to a scrollView instance.
   *
   * This will be called once.
   *
   * @param[in] scrollView The attached scrollView instance.
   */
  virtual void OnAttach(UI::ScrollView& scrollView) = 0;

  /**
   * Called upon Detaching of effect from a scrollView instance.
   *
   * This will be called once.
   *
   * @param[in] scrollView The attached scrollView instance.
   */
  virtual void OnDetach(UI::ScrollView& scrollView) = 0;

protected:
  /**
   * Returns the ScrollView handle that this effect is
   * attached to.
   * @note if it's not attached to any ScrollView then
   * will return an uninitialized handle.
   * @return The scrollView handle is returned.
   */
  UI::ScrollView GetScrollView();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollViewEffect();

private:
  UI::Internal::ScrollView* mScrollViewImpl; ///< Attached ScrollView instance (pointer to implementation)
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ScrollViewEffect& GetImpl(Dali::UI::ScrollViewEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<Internal::ScrollViewEffect&>(handle);
}

inline const Internal::ScrollViewEffect& GetImpl(const Dali::UI::ScrollViewEffect& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::RefObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::ScrollViewEffect&>(handle);
}

} // namespace UI

} // namespace Dali

#endif // DALI_UI_INTERNAL_SCROLL_VIEW_EFFECT_H
