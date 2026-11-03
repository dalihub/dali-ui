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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <unordered_map>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

// Forward declarations
class View;
class ViewImpl;

namespace Internal
{

/**
 * @brief Internal implementation of LayoutScaleManager.
 *
 * Stores the current system scale, a scale per Window and a list of layout root weak
 * handles. On a scale or master-switch change, all registered roots are invalidated and
 * re-layout is triggered; on a window-scale change only the roots that currently live in
 * that window are (and none while scaling is globally disabled; SetScalable(true)
 * applies it).
 *
 * This object is the only owner of that state in the process. RegisterLayoutRoot() and
 * UnregisterLayoutRoot() are reached through GetImpl(); the Extension::LayoutScaleManager
 * handle does not expose them.
 */
class DALI_UI_API LayoutScaleManagerImpl : public BaseObject
{
public:
  /**
   * @brief Returns the singleton LayoutScaleManager handle.
   *
   * Creates the implementation on first call. The singleton is a function-local
   * static, so the handle is never empty, also before the adaptor or Core exist.
   *
   * @return A LayoutScaleManager handle wrapping the singleton impl
   */
  static Ui::Extension::LayoutScaleManager Get();

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::GetUiScale
   */
  float GetUiScale() const;

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::SetUiScale
   */
  void SetUiScale(float scale);

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::IsScalable
   */
  bool IsScalable() const;

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::SetScalable
   */
  void SetScalable(bool enable);

  /**
   * @brief Registers a layout root view to be invalidated on scale change.
   *
   * Called when a view becomes a layout root (no parent layout/view).
   *
   * Also prunes the window-scale records whose window has been destroyed. It is
   * the hook for a window destroyed with NO layout root, which never reaches
   * UnregisterLayoutRoot() for itself; see PruneDeadWindowScales().
   *
   * @param[in] root The root view to register
   */
  void RegisterLayoutRoot(Ui::View root);

  /**
   * @brief Unregisters a layout root view.
   *
   * Prunes the dead window-scale records too, as RegisterLayoutRoot() does. This
   * is the prune that normally reclaims a destroyed window's record, inside that
   * window's own teardown; see PruneDeadWindowScales().
   *
   * @param[in] root The root view to unregister
   */
  void UnregisterLayoutRoot(Ui::View root);

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::GetWindowScale
   */
  float GetWindowScale(Dali::Window window) const;

  /**
   * @copydoc Dali::Ui::Extension::LayoutScaleManager::SetWindowScale
   */
  void SetWindowScale(Dali::Window window, float scale);

  /**
   * @brief Returns whether any per-window scale record exists.
   *
   * The gate every effective-scale resolution tests BEFORE calling
   * ResolveWindowScale(), and before the policy ascent that decides whether to call it.
   * It is inline and reads a single static bool, so while no application has ever set
   * a window scale -- the default for every application -- the window term of an
   * effective-scale cache miss pays one load and one branch and nothing else: no
   * singleton handle, no actor temporary, no Window lookup and no ascent. That is what
   * keeps the cost claims in the comments above GetSystemScale() and
   * ResolveWindowScaleTerm() (view-data-impl.cpp) true, and it is the same
   * inline-static-gate shape as LayoutTransitionImpl::HasAnyInstance().
   *
   * "Any record exists", not "any LIVE record exists": a window destroyed without a
   * reset leaves an inert entry until the next prune, and the gate stays armed for
   * that long. Nothing is resolved incorrectly -- an expired entry can never match a
   * live window -- the window lookup is merely still paid. That interval is usually
   * zero or short. A window that still has layout roots loses its record inside its
   * own teardown, to the UnregisterLayoutRoot() prune; one destroyed with no layout
   * root loses it at the next RegisterLayoutRoot(), UnregisterLayoutRoot() or
   * SetWindowScale(), and RegisterLayoutRoot() is on the path that schedules
   * nearly every layout pass. (LayoutController::RequestLayout(), for one, schedules a
   * pass without registering a root.)
   *
   * @return True when at least one window has a recorded scale
   */
  static bool HasAnyWindowScale()
  {
    return gHasWindowScales;
  }

  /**
   * @brief Resolves the per-window scale that applies to @p view.
   *
   * The resolution itself, and deliberately NOT the gate: callers test
   * HasAnyWindowScale() first so the common no-window-scale case never reaches this
   * function. Calling it ungated is still correct -- with no record for the view's
   * window it returns 1.0f -- it just pays the actor temporary, the Window lookup and
   * the singleton handle to learn that.
   *
   * It is also THE master-switch read for the window term, the counterpart of
   * GetSystemScale() (view-data-impl.cpp) for the system term: while
   * LayoutScaleManager::SetScalable(false) is in force it returns 1.0f whatever is recorded.
   *
   * @param[in] view The view whose window to resolve
   * @return That window's scale; 1.0f when no scale is recorded for it, when the view is
   *         not currently in a Window (off-scene, or under a non-Window SceneHolder), or
   *         while scaling is globally disabled
   */
  static float ResolveWindowScale(const ViewImpl& view);

protected:
  LayoutScaleManagerImpl();
  ~LayoutScaleManagerImpl() override;

private:
  LayoutScaleManagerImpl(const LayoutScaleManagerImpl&)            = delete;
  LayoutScaleManagerImpl(LayoutScaleManagerImpl&&)                 = delete;
  LayoutScaleManagerImpl& operator=(const LayoutScaleManagerImpl&) = delete;
  LayoutScaleManagerImpl& operator=(LayoutScaleManagerImpl&&)      = delete;

  // Drops the cached scale + layout caches of every registered layout root's
  // subtree and re-triggers layout. Shared by SetUiScale and SetScalable.
  void InvalidateAllLayoutRoots();

  // The per-root half of the two invalidation walks: drop the root's whole subtree
  // scale + layout caches, then re-arm its layout pass. Extracted so the global walk
  // (InvalidateAllLayoutRoots) and the per-window walk (InvalidateLayoutRootsOfWindow)
  // cannot drift apart.
  static void ResetAndInvalidateLayoutRoot(Ui::View rootView);

  // Drops the cached scale + layout caches of every registered layout root that
  // currently lives in `window`, and re-triggers layout for those roots only.
  void InvalidateLayoutRootsOfWindow(Dali::Window window);

  // Drops the records whose window has been destroyed, so an application never has to
  // reset a window scale before destroying its window. Called from the growth point
  // (SetWindowScale, which prunes BEFORE it grows) and from the two layout-root
  // hooks (RegisterLayoutRoot, UnregisterLayoutRoot) -- never from a lookup, which
  // runs inside the const lazy fill of ViewDataImpl::GetEffectiveLayoutScale() and must not
  // mutate manager state.
  //
  // A record is reclaimed by the first prune after its window's
  // BaseObject::OnDestroy(), which nulls the weak handles before any destructor of the
  // window runs. For a window that still has layout roots, that first prune is in its
  // own teardown: its views disconnect while ~SceneHolder's members are being
  // destroyed, and the UnregisterLayoutRoot() each root goes through reclaims the
  // record there. A window destroyed with no layout root waits for the next
  // RegisterLayoutRoot(), UnregisterLayoutRoot() or SetWindowScale().
  //
  // Free when no window scale was ever set: it early-outs on an empty vector.
  void PruneDeadWindowScales();

private:
  /// One window's scale. The window is held WEAKLY: this record must neither extend a
  /// window's lifetime nor survive it as a live scale.
  struct WindowScaleEntry
  {
    WeakHandle<Dali::Window> window; ///< Object identity without extending Window lifetime.
    float                    scale{1.0f};
  };

  /// Mirror of !mWindowScales.empty(), read through HasAnyWindowScale().
  /// A static rather than a member because the gate must not cost the singleton
  /// handle it would take to reach the member; there is exactly one
  /// LayoutScaleManagerImpl instance (see Get()), so the mirror is well defined.
  static bool gHasWindowScales;

  float                                            mUiScale{1.0f};
  bool                                             mIsScalable{true};
  std::unordered_map<RefObject*, WeakHandle<View>> mLayoutRoots;
  std::vector<WindowScaleEntry>                    mWindowScales; ///< Non-default per-window scales only.
};

} // namespace Internal

namespace Extension
{
/**
 * @brief Retrieves the LayoutScaleManagerImpl from an Extension::LayoutScaleManager handle.
 *
 * @param[in] obj The LayoutScaleManager handle
 * @return A reference to the internal implementation
 */
inline Ui::Internal::LayoutScaleManagerImpl& GetImpl(LayoutScaleManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Ui::Internal::LayoutScaleManagerImpl&>(handle);
}

/**
 * @brief Retrieves the LayoutScaleManagerImpl from a const Extension::LayoutScaleManager handle.
 *
 * @param[in] obj The LayoutScaleManager handle
 * @return A const reference to the internal implementation
 */
inline const Ui::Internal::LayoutScaleManagerImpl& GetImpl(const LayoutScaleManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Ui::Internal::LayoutScaleManagerImpl&>(handle);
}

} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE
