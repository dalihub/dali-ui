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

// CLASS HEADER
#include <dali-ui-foundation/internal/layout-scale-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

bool LayoutScaleManagerImpl::gHasWindowScales = false;

namespace
{
// "Has this record's window been destroyed?", answered WITHOUT materialising a strong
// handle -- and deliberately NOT as the `!entry.window.GetHandle()` that the rest of
// this library spells an expiry test with (FocusManager's independent-focus records,
// focus-manager-impl.cpp). The reason is cost and simplicity. Comparing two WeakHandles
// compares their stored object pointers and nothing else (WeakHandleBase::operator==,
// dali-core weak-handle.cpp): no reference count is touched, no handle is constructed
// and no observer is registered, so no destructor and no callback can run inside the
// test, and nothing can re-enter the manager while the prune walks mWindowScales.
// GetHandle() would construct a strong handle for every live record on every prune --
// a Reference()/Unreference() pair and a DownCast each -- and the prune runs from the
// layout-root hooks.
//
// A record reads as expired once its window's BaseObject::OnDestroy() has run, and that
// is before any destructor of the window. RefObject::Unreference() calls OnDestroy()
// BEFORE `delete this` (dali-core ref-object.cpp; documented on RefObject::OnDestroy),
// and BaseObject::OnDestroy() deletes the object's Impl, whose destructor notifies
// every observer, so each WeakHandle nulls its stored pointer right there (dali-core
// base-object.cpp, base-object-impl.cpp and weak-handle.cpp; dali-core pins the order
// with UtcDaliWeakHandleInvalidDuringSelfDestruction). A dying window's views
// disconnect later, while ~SceneHolder's members are being destroyed, so the record
// already reads as expired -- and a GetHandle() of it would already be EMPTY -- when
// the UnregisterLayoutRoot() prune those disconnections drive runs. That prune reclaims
// it inside the window's own teardown.
//
// The empty comparand is allocated on the first prune that has a record to test, because
// WeakHandle's default constructor allocates its Impl: one lazy allocation for the whole
// process rather than one per prune. It observes nothing -- its stored pointer is null, so
// no observer was ever registered. It is deliberately never destroyed. A function-local
// static OBJECT would be constructed after this manager's static and therefore destroyed
// before it at exit, and a prune that still runs during static destruction -- a window torn
// down then, or a held handle used then -- would compare against its freed Impl.
bool IsWindowRecordExpired(const WeakHandle<Dali::Window>& window)
{
  static const WeakHandle<Dali::Window>* const EXPIRED = new WeakHandle<Dali::Window>();
  return window == *EXPIRED;
}

} // namespace

LayoutScaleManagerImpl::LayoutScaleManagerImpl() = default;

LayoutScaleManagerImpl::~LayoutScaleManagerImpl() = default;

LayoutScaleManager LayoutScaleManagerImpl::Get()
{
  static IntrusivePtr<LayoutScaleManagerImpl> impl;

  if(!impl)
  {
    impl = new LayoutScaleManagerImpl();
  }

  return LayoutScaleManager(impl.Get());
}

float LayoutScaleManagerImpl::GetUiScale() const
{
  return mUiScale;
}

void LayoutScaleManagerImpl::SetUiScale(float scale)
{
  // !isfinite rather than isnan: the documented contract is a positive FINITE
  // scale, and an infinite scale multiplies straight into every view's effective
  // scale, so every measured size and arranged rect in the tree becomes infinite.
  // isnan alone let +/-Inf through (Inf > 0 and is not NaN).
  //
  // The lower bound is FLT_MIN, not 0: the layout divides by the effective scale in a
  // dozen places, and a subnormal scale makes 1 / scale overflow to infinity long
  // before the scale itself is zero. A subnormal is not a usable display scale, so it
  // is rejected with every other invalid value rather than clamped.
  if(!std::isfinite(scale) || scale < std::numeric_limits<float>::min())
  {
    DALI_LOG_ERROR("LayoutScaleManagerImpl::SetUiScale: invalid scale value (%f). Scale must be a positive, finite, normal number (>= FLT_MIN).\n", scale);
    return;
  }

  if(mUiScale == scale)
  {
    return;
  }

  mUiScale = scale;

  // While scaling is disabled the stored scale has no effect on any view's
  // effective scale (ComputeEffectiveScale collapses to 1.0f), so applying it
  // now would relayout the whole tree for no visible change. Store it and defer
  // the relayout to SetScalable(true), which re-applies the stored scale.
  if(mIsScalable)
  {
    InvalidateAllLayoutRoots();
  }
}

bool LayoutScaleManagerImpl::IsScalable() const
{
  return mIsScalable;
}

void LayoutScaleManagerImpl::SetScalable(bool enable)
{
  if(mIsScalable == enable)
  {
    return;
  }

  mIsScalable = enable;

  // Both directions move every scaled view's effective scale (enable: 1.0 -> the stored
  // window scale x the stored system scale, disable: back to 1.0) -- BOTH terms, since
  // the switch gates the per-window scale too -- so a full subtree reset plus a
  // re-layout is required either way. This global walk is also what applies a window
  // scale SetWindowScale() recorded while the switch was off: that call deferred its own,
  // per-window invalidation to here.
  InvalidateAllLayoutRoots();
}

void LayoutScaleManagerImpl::InvalidateAllLayoutRoots()
{
  // Invalidate and re-trigger layout for all registered roots
  auto it = mLayoutRoots.begin();
  while(it != mLayoutRoots.end())
  {
    Ui::View rootView = it->second.GetHandle();
    if(!rootView)
    {
      it = mLayoutRoots.erase(it);
      continue;
    }

    ResetAndInvalidateLayoutRoot(rootView);

    ++it;
  }
}

void LayoutScaleManagerImpl::ResetAndInvalidateLayoutRoot(Ui::View rootView)
{
  ViewImpl& viewImpl = GetImpl(rootView);
  // Drop the cached scale (and the layout caches derived from it) for the
  // entire subtree first, so every view re-evaluates its effective scale on
  // the next Measure pass.
  ViewDataImpl::Get(viewImpl).ResetSubtreeScaleAndLayoutCaches();
  // Then invalidate measure at the root to schedule a re-layout pass.
  // Through the internal primitive, not ViewImpl::InvalidateMeasure(): a
  // scale change is a framework-internal event, not an application call.
  ViewDataImpl::Get(viewImpl).InvalidateMeasure();
}

void LayoutScaleManagerImpl::InvalidateLayoutRootsOfWindow(Dali::Window window)
{
  // The same walk, the same stale-entry pruning and the same per-root action as
  // InvalidateAllLayoutRoots, narrowed to the roots that currently live in `window`.
  //
  // Each root's window is asked for HERE rather than remembered at registration
  // time. A remembered window would be a second class of stale state to keep fresh
  // -- a root's window changes on reparent and on scene reconnection, and the
  // stale-address rebinding in RegisterLayoutRoot would have to refresh it too --
  // and storing it would put a WeakHandle construction on the registration path,
  // which every InvalidateMeasure that walks up to a root goes through. The lookup
  // here is paid only when an application changes a window scale while scaling is
  // enabled (a change made while it is disabled is applied by SetScalable(true)'s
  // global walk), and a MATCHED root then pays a whole-subtree reset plus a
  // re-measure, which dominates it.
  auto it = mLayoutRoots.begin();
  while(it != mLayoutRoots.end())
  {
    Ui::View rootView = it->second.GetHandle();
    if(!rootView)
    {
      it = mLayoutRoots.erase(it);
      continue;
    }

    if(Dali::Window::Get(rootView) == window)
    {
      ResetAndInvalidateLayoutRoot(rootView);
    }

    ++it;
  }
}

void LayoutScaleManagerImpl::RegisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  const auto key      = root.GetObjectPtr();
  auto [it, inserted] = mLayoutRoots.try_emplace(key, root);
  if(!inserted && !it->second.GetHandle())
  {
    // Replace a stale weak entry when a newly allocated view reuses an old
    // root's address. A live root is already registered, so leave its
    // WeakHandle intact and avoid allocation on duplicate registration.
    it->second = WeakHandle<Ui::View>(root);
  }

  // The cleanup hook for the records of windows destroyed with NO layout root. A window
  // that still has roots loses its record inside its own teardown, to the prune in
  // UnregisterLayoutRoot(); a rootless one never comes through either hook for itself,
  // so its record waits for the next prune, and this hook is what keeps that wait
  // short. Both callers inside the library -- RegisterWithLayoutController, right after
  // it requests the layout pass, and the standalone branch of OnViewSceneConnection --
  // reach here through a LIVE window only, so this prune runs whenever a view connects
  // as, or an invalidation reaches, a layout root in a live window. The first such
  // event after the window is gone -- the next window's first view, say -- drops the
  // record without the application resetting anything. Free when no window scale was
  // ever set; see PruneDeadWindowScales().
  //
  // Nothing of mWindowScales is live on the stack when this re-enters from
  // SetWindowScale (-> InvalidateLayoutRootsOfWindow -> InvalidateMeasure ->
  // RegisterWithLayoutController -> here); the ordering note at that call site is what
  // keeps it so.
  PruneDeadWindowScales();
}

void LayoutScaleManagerImpl::UnregisterLayoutRoot(Ui::View root)
{
  if(!root)
  {
    return;
  }

  mLayoutRoots.erase(root.GetObjectPtr());

  // The prune that NORMALLY reclaims a destroyed window's record, right inside that
  // window's own teardown. The window's views disconnect while ~SceneHolder's members
  // are being destroyed, every layout root among them comes through here, and by then
  // the window's BaseObject::OnDestroy() has already nulled the weak handles, so the
  // record reads as expired (see IsWindowRecordExpired()). A window that has no layout
  // root left when it is destroyed never comes through here for itself; its record
  // waits for the next RegisterLayoutRoot(), UnregisterLayoutRoot() or
  // SetWindowScale().
  PruneDeadWindowScales();
}

float LayoutScaleManagerImpl::GetWindowScale(Dali::Window window) const
{
  if(window)
  {
    for(const auto& entry : mWindowScales)
    {
      // Compare on OBJECT IDENTITY, never on a raw address: WeakHandle nulls its
      // object pointer when the window is destroyed, so an expired entry can never
      // compare equal to a live window, whatever the allocator later puts at that
      // address. That is why this is not keyed on a RefObject* the way mLayoutRoots
      // is: a raw key needs explicit stale-address rebinding to stay correct, and
      // rebinding an application-authored scale onto an unrelated new window is not
      // a recovery, it is a wrong answer. A record that outlives its window must
      // simply stop matching.
      //
      // The STRONG handle spells that identity here, unlike the prune's handle-free
      // IsWindowRecordExpired(), because a handle-free MATCH would need a WeakHandle
      // of `window` to compare against, built on every call -- and building one
      // allocates its Impl and registers a destruction observer, on the const,
      // allocation-free lazy fill of ViewDataImpl::GetEffectiveLayoutScale().
      if(entry.window.GetHandle() == window)
      {
        return entry.scale;
      }
    }
  }

  // No record, or no window to look one up for: the documented default.
  return 1.0f;
}

void LayoutScaleManagerImpl::SetWindowScale(Dali::Window window, float scale)
{
  if(!window)
  {
    DALI_LOG_ERROR("LayoutScaleManagerImpl::SetWindowScale: empty window. The window scale applies to the views of one window, so there is no window to apply it to.\n");
    return;
  }

  // The same bounds as SetUiScale, for the same reason: this window scale is a term of the
  // very effective scale the layout divides by in a dozen places, so an infinite window
  // scale makes every measured size and arranged rect infinite, and a subnormal one makes
  // 1 / scale overflow to infinity long before the window scale itself is zero.
  //
  // This checks the TERM, not the product it forms with mUiScale, and deliberately so:
  // validating scale * mUiScale here would make this call's outcome depend on the order
  // the two knobs were set in -- the same pair accepted one way and rejected the other
  // -- and SetUiScale cannot symmetrically re-validate against every window's scale
  // either. Both terms passing is therefore not a proof that the product is finite and
  // normal (1e20 * 1e20 overflows, 1e-25 * 1e-25 underflows to zero); that is the
  // caller's contract, stated on LayoutScaleManager::SetWindowScale.
  if(!std::isfinite(scale) || scale < std::numeric_limits<float>::min())
  {
    DALI_LOG_ERROR("LayoutScaleManagerImpl::SetWindowScale: invalid window scale value (%f). The window scale must be a positive, finite, normal number (>= FLT_MIN).\n", scale);
    return;
  }

  // Prune FIRST, so the no-op check and the scan below see live records only. This is
  // the growth point, and pruning before growing is what bounds the records a
  // create-scale-destroy loop can leave behind -- and RegisterLayoutRoot and
  // UnregisterLayoutRoot prune too, which is what makes a destroyed window's record
  // disappear without the application resetting it first. Lookups still deliberately do
  // not prune -- GetWindowScale() runs inside the const lazy fill of
  // ViewDataImpl::GetEffectiveLayoutScale(), which must not mutate manager state.
  PruneDeadWindowScales();

  // Exact ==, as SetUiScale compares its own scale: the window scale is stored and read
  // back verbatim and the measure cache's scale key is compared exactly too, so an
  // epsilon here would relayout the window for a change no cache can observe.
  if(GetWindowScale(window) == scale)
  {
    return;
  }

  // The same linear scan as GetWindowScale and the same strong-handle MATCH
  // comparison, so this finds exactly the record the no-op check above has just read;
  // it is spelled out rather than hoisted into a predicate because the branches below
  // need the ITERATOR, not just the value.
  auto it = mWindowScales.begin();
  while(it != mWindowScales.end() && it->window.GetHandle() != window)
  {
    ++it;
  }

  if(scale == 1.0f)
  {
    // 1.0 is exactly the documented default and is exactly representable, so the
    // reset is a plain SetWindowScale(window, 1.0f) and it ERASES the record
    // rather than storing a 1.0f: an empty vector is what disarms the
    // no-window-scale-anywhere gate, HasAnyWindowScale(). The no-op
    // check above already returned for a window with no record, and a stored scale
    // is never 1.0f (this branch is what keeps it so), so the record is there; the
    // guard is defence, not a case.
    if(it != mWindowScales.end())
    {
      mWindowScales.erase(it);
    }
  }
  else if(it != mWindowScales.end())
  {
    it->scale = scale;
  }
  else
  {
    mWindowScales.push_back(WindowScaleEntry{WeakHandle<Dali::Window>(window), scale});
  }

  gHasWindowScales = !mWindowScales.empty();

  // ORDERING, load-bearing: `it` above must be finished with BEFORE the invalidation
  // below. Invalidation re-enters RegisterLayoutRoot (ResetAndInvalidateLayoutRoot ->
  // ViewDataImpl::InvalidateMeasure -> RegisterWithLayoutController ->
  // RegisterLayoutRoot), which prunes mWindowScales and can therefore erase from it,
  // and an erase invalidates every iterator at or after the erased record. `it` is
  // still in scope here but dead; reading it after this point would be a
  // use-after-invalidation. The mirror write above must stay above the invalidation for
  // the same reason -- a re-entrant prune must not see a stale mirror.
  //
  // DEFERRED while scaling is globally disabled -- the deferral SetUiScale applies, for the
  // same reason. While the switch is off every view's effective scale is 1.0, this term
  // included (ResolveWindowScale() returns 1.0f), so relayouting this window now would
  // recompute every one of its views to the value it already has. The record is stored
  // or erased above all the same -- GetWindowScale() reports it and the
  // HasAnyWindowScale() gate follows it -- and SetScalable(true) applies it, because
  // re-enabling invalidates EVERY registered layout root, this window's included.
  if(mIsScalable)
  {
    InvalidateLayoutRootsOfWindow(window);
  }
}

void LayoutScaleManagerImpl::PruneDeadWindowScales()
{
  // The early-out that keeps this free for every application that never sets a window
  // scale: an empty() test and one branch. No such application can ever get past it,
  // because the ONLY place a record is created is the push_back in SetWindowScale.
  // Skipping the mirror write below is safe precisely because the vector can only BE
  // empty through a write that already set the mirror false -- SetWindowScale, or the
  // tail of this function.
  if(mWindowScales.empty())
  {
    return;
  }

  // Erase-while-iterating, the same shape the layout-root walks in this file use: a
  // record whose window is gone can never match again, so it is dropped rather than
  // kept as a never-matching entry that holds the HasAnyWindowScale() gate armed.
  // The expiry test is handle-free on purpose; see IsWindowRecordExpired().
  //
  // WHY THE LEFTOVER RECORDS ARE BOUNDED. SetWindowScale prunes BEFORE it grows the
  // vector, and it is the only growth point, so "create a window, give it a scale,
  // destroy it, repeat" cannot accumulate: each new window's scale-set call reclaims
  // whatever the previous window left. A window that still has a layout root leaves
  // nothing: it loses its record inside its own teardown (see UnregisterLayoutRoot).
  // RegisterLayoutRoot covers the rest -- a process that destroys a rootless
  // window-scale-bearing window and then keeps doing UI work without setting any new
  // window scale.
  //
  // The one residual, stated rather than papered over: a window destroyed with NO
  // layout root, followed by no further layout or window-scale activity at all -- no
  // RegisterLayoutRoot, UnregisterLayoutRoot or SetWindowScale call -- keeps its
  // record until the process exits. That costs memory -- tens of bytes per such window
  // -- and keeps the HasAnyWindowScale() gate armed, but nothing resolves incorrectly
  // meanwhile, because an expired record can never match a live window (see
  // GetWindowScale).
  auto it = mWindowScales.begin();
  while(it != mWindowScales.end())
  {
    if(IsWindowRecordExpired(it->window))
    {
      it = mWindowScales.erase(it);
    }
    else
    {
      ++it;
    }
  }

  // Re-mirror here too: a prune that empties the vector must disarm the gate, not
  // leave it armed until the next store.
  gHasWindowScales = !mWindowScales.empty();
}

float LayoutScaleManagerImpl::ResolveWindowScale(const ViewImpl& view)
{
  // Window::Get is the only path from an actor to its window (Scene::Get, plus a
  // linear scan of the adaptor's windows and a dynamic_cast); SceneHolder::Get goes
  // through the same function. It yields an EMPTY handle for an actor with no window,
  // which is also what a non-Window SceneHolder yields, and both mean "no window
  // scale applies" rather than "look one up".
  Dali::Window window = Dali::Window::Get(view.Self());
  if(!window)
  {
    return 1.0f;
  }

  LayoutScaleManager            manager = LayoutScaleManagerImpl::Get();
  const LayoutScaleManagerImpl& impl    = GetImpl(manager);

  // THE master-switch read for the window term, the counterpart of the one in
  // GetSystemScale() (view-data-impl.cpp) for the system term: while scaling is globally
  // disabled every view's effective scale is 1.0, this term included, and the record is
  // left untouched for SetScalable(true) to re-apply (it invalidates every layout root).
  // Tested here, on the handle this function already takes and after the Window lookup,
  // so no path that existed before the switch gated this term pays anything new: an
  // application with no window scale never gets here (the inline HasAnyWindowScale()
  // gate in front of every call), and a view with no window still returns above without
  // touching the singleton.
  if(!impl.IsScalable())
  {
    return 1.0f;
  }
  return impl.GetWindowScale(window);
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
