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
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-transition-dispatcher.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{

namespace
{
// File-static map to store LayoutController instances per Window (internal use only; not exposed in public API).
std::unordered_map<void*, std::unique_ptr<LayoutController>> gLayoutControllers;
} // namespace

namespace Integration
{

/**
 * @brief Internal implementation of LayoutController.
 *
 * Implements the Integration::Processor interface to hook into
 * DALi's per-frame processing pipeline.
 */
class LayoutControllerImpl : public Dali::Integration::Processor, public ConnectionTracker
{
public:
  /**
   * @brief Data for a tracked layout root.
   *
   * Stores a weak handle (non-ref-counted) for validity checking and
   * the raw implementation pointer for fast access during layout passes.
   */
  struct LayoutRootEntry
  {
    WeakHandle<View> weakHandle; ///< Non-ref-counted weak reference; auto-nullified on destruction
    ViewImpl*        view;       ///< Raw pointer for direct access
  };

  /**
   * @brief Constructor.
   */
  explicit LayoutControllerImpl(Window window)
  : mWindow(window),
    mTransitionDispatcher(new Internal::LayoutTransitionDispatcher()),
    mWindowWidth(0),
    mWindowHeight(0),
    mWindowObjectPtr(window.GetObjectPtr()),
    mProcessingScheduled(false)
  {
    // Get initial window size
    Vector2 size  = window.GetSize();
    mWindowWidth  = static_cast<int32_t>(size.width);
    mWindowHeight = static_cast<int32_t>(size.height);

    // Register as a processor with the adaptor (postProcess=false: run before dali Relayout)
    if(DALI_LIKELY(Adaptor::IsAvailable()))
    {
      Adaptor::Get().RegisterProcessor(*this, false);

      // Connect to window resize signal
      window.ResizeSignal().Connect(this, &LayoutControllerImpl::OnWindowResized);
    }
  }

  /**
   * @brief Destructor.
   */
  ~LayoutControllerImpl() override
  {
    // Unregister from adaptor
    if(DALI_LIKELY(Adaptor::IsAvailable()))
    {
      Adaptor::Get().UnregisterProcessor(*this);
    }
  }

  /**
   * @brief Schedules a view for layout processing.
   *
   * Any View that is a layout root (has LayoutManager or children) can be registered.
   */
  void RequestLayout(ViewImpl* view)
  {
    if(!view)
    {
      return;
    }

    // Track this layout root (weak handle for validity checking without extending lifetime)
    View handle           = View::DownCast(view->Self());
    mAllLayoutRoots[view] = LayoutRootEntry{WeakHandle<View>(handle), view};

    // Add to pending (dirty) set
    mPendingViews.insert(view);

    // Schedule processing if not already scheduled
    if(!mProcessingScheduled)
    {
      mProcessingScheduled = true;
    }
  }

  /**
   * @brief Schedules an EXIT-slot transition (forwarded from ViewImpl::RemoveChild).
   */
  void ScheduleLayoutExit(ViewImpl* parent, Ui::View child, ViewImpl* transitionOwner)
  {
    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->ScheduleExit(parent, child, transitionOwner);
    }
  }

  /**
   * @brief Notifies the dispatcher that @p child was just attached to a
   * (new) parent, so any in-flight transition under the old parent can be
   * cancelled before the new parent dispatches ENTER.
   *
   * Forwarded from @c ViewImpl::OnChildAdd. No-op when @p child has no
   * in-flight transition (the common fresh-add case).
   */
  void NotifyChildReparented(ViewImpl* child)
  {
    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->OnChildReparented(child);
    }
  }

  void NotifyChildAdded(ViewImpl* directParent, Ui::View child)
  {
    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->NotifyChildAdded(directParent, child);
    }
  }

  void ClearPendingInheritedEnters(ViewImpl* owner)
  {
    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->ClearPendingInheritedEnters(owner);
    }
  }

  /**
   * @brief Removes a view from tracking (called when view is destroyed).
   */
  void UnregisterView(ViewImpl* view)
  {
    mAllLayoutRoots.erase(view);
    mPendingViews.erase(view);
    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->OnViewDestroyed(view);
    }
  }

  /**
   * @brief Gets the current window handle managed by this layout controller.
   *
   * Retrieves the window that this layout controller instance is associated with.
   * This is used internally to verify if the window has been replaced.
   *
   * @return The current window handle
   */
  Dali::Window GetCurrentWindow() const
  {
    return mWindow.GetHandle();
  }

  /**
   * @brief Replaces the current window with a new one.
   *
   * Updates the layout controller to manage a different window instance.
   * This is called when a window object has been replaced but the same
   * LayoutController instance should continue managing layouts for the new window.
   * The method reconnects the window resize signal to ensure layout invalidation
   * continues to work correctly.
   *
   * @param[in] window The new window to manage
   */
  void ReplaceCurrentWindow(Dali::Window window)
  {
    DALI_ASSERT_ALWAYS(mWindowObjectPtr == window.GetObjectPtr() && "ReplaceCurrentWindow should be called only for same object ptr case!");

    mWindow = window;
    if(DALI_LIKELY(Adaptor::IsAvailable()))
    {
      // Connect to window resize signal
      window.ResizeSignal().Connect(this, &LayoutControllerImpl::OnWindowResized);
    }
  }

  /**
   * @brief Called when window is resized.
   */
  void OnWindowResize(int32_t width, int32_t height)
  {
    mWindowWidth  = width;
    mWindowHeight = height;

    if(mTransitionDispatcher)
    {
      mTransitionDispatcher->NotifyWindowResize();
    }

    // Invalidate ALL known layout roots (not just pending ones)
    // Collect dead entries to remove after iteration
    std::vector<ViewImpl*> deadEntries;
    for(auto& pair : mAllLayoutRoots)
    {
      if(pair.second.weakHandle.GetHandle())
      {
        pair.second.view->InvalidateMeasure();
      }
      else
      {
        deadEntries.push_back(pair.first);
      }
    }
    for(auto* dead : deadEntries)
    {
      mAllLayoutRoots.erase(dead);
      mPendingViews.erase(dead);
    }
  }
  /**
   * @brief Processes all pending views with layout capability.
   */
  void ProcessLayouts()
  {
    Process(false);
  }

  /**
   * @brief Implementation of Integration::Processor::Process.
   *
   * Called once per frame by DALi's adaptor.
   */
  void Process(bool postProcess) override
  {
    Dali::Window window = mWindow.GetHandle();
    if(DALI_UNLIKELY(!window))
    {
      // Destroy self.
      gLayoutControllers.erase(mWindowObjectPtr);

      // Don't do any extra process after self destructor called.
      return;
    }

    if(!postProcess)
    {
      ProcessLayouts(window);

      if(mTransitionDispatcher)
      {
        // The dispatcher computes deltaSec from its own wall clock and,
        // once an animator becomes active, also drives a periodic tick
        // timer so subsequent ticks fire even when no other event wakes
        // the event thread.
        mTransitionDispatcher->TickAnimators();
      }
    }
  }

  /**
   * @brief Implementation of Integration::Processor::GetProcessorName.
   */
  std::string_view GetProcessorName() const override
  {
    return "Ui::LayoutController";
  }

private:
  /**
   * @brief Processes all pending views with layout capability.
   */
  void ProcessLayouts(Dali::Window window)
  {
    if(mTransitionDispatcher)
    {
      // Mark the start of this batch. Paired with EndLayoutPass at every
      // exit point below, this protects per-pass flags (mInWindowResize)
      // from being cleared by a nested ProcessLayouts call invoked from
      // a lifecycle callback during a window-resize-driven pass.
      mTransitionDispatcher->BeginLayoutPass();
    }
    if(mPendingViews.empty())
    {
      // Still drain per-pass dispatcher state (e.g. mInWindowResize set
      // by NotifyWindowResize) so a stale flag cannot leak into the next
      // pass when every layout root happens to be dead at resize time.
      if(mTransitionDispatcher)
      {
        mTransitionDispatcher->EndLayoutPass();
      }
      return;
    }

    // Copy pending views and clear (in case new views are added during processing)
    decltype(mPendingViews) viewsSet;
    viewsSet.swap(mPendingViews);
    mProcessingScheduled = false;

    // Default: window size (when root is directly under window or parent size unknown).
    Vector2 windowSize       = window.GetSize();
    float   widthConstraint  = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.width)));
    float   heightConstraint = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.height)));

    // Sort pending roots by tree depth (outer-most first). When a parent
    // and a descendant root are both in the batch (e.g. a transition-
    // bearing parent and one of its standalone children that is its own
    // layout root), the parent must run first so its arrange pass can
    // measure the descendant via MeasureStandaloneChildren / parent's
    // recursive arrange. Without this ordering the descendant would
    // measure with a stale or zero parent SIZE, dispatch its transitions
    // with bad bounds, and then have those transitions cancelled when
    // the parent's later pass corrects the bounds.
    std::vector<ViewImpl*> viewsToProcess(viewsSet.begin(), viewsSet.end());
    {
      auto depthOf = [](ViewImpl* v) -> int
      {
        int d = 0;
        if(v)
        {
          for(Dali::Actor p = v->Self().GetParent(); p; p = p.GetParent())
          {
            ++d;
          }
        }
        return d;
      };
      std::sort(viewsToProcess.begin(), viewsToProcess.end(),
                [&depthOf](ViewImpl* a, ViewImpl* b)
      {
        return depthOf(a) < depthOf(b);
      });
    }

    // Process each layout root
    for(auto* view : viewsToProcess)
    {
      // Verify the view is still tracked and alive
      auto it = mAllLayoutRoots.find(view);
      if(view && it != mAllLayoutRoots.end() && it->second.weakHandle.GetHandle())
      {
        if(mTransitionDispatcher)
        {
          mTransitionDispatcher->CaptureBeforeLayout(view);
        }
        ProcessLayoutRoot(view, widthConstraint, heightConstraint);
        if(mTransitionDispatcher)
        {
          mTransitionDispatcher->StartTransitionsAfterLayout(view);
        }
      }
      else if(it != mAllLayoutRoots.end())
      {
        // Dead entry — clean up
        mAllLayoutRoots.erase(it);
      }
    }

    if(mTransitionDispatcher)
    {
      // Reset per-pass flags after the whole batch. Per-root reset would
      // clear them after the first root and misclassify the rest.
      mTransitionDispatcher->EndLayoutPass();
    }
  }

  /**
   * @brief Processes a single view with layout capability (layout root).
   *
   * Constraint priority chain (per-dimension):
   *   1. Explicit RequestedWidth/Height (>= 0): use that value.
   *   2. Else if the view's Actor has a parent: use parent.Actor.SIZE (as-is,
   *      including 0). When this view is a standalone (boundary) child, a
   *      parent size of 0 means the parent has not been arranged yet —
   *      measuring with 0 is correct because the parent's subsequent
   *      Measure/Arrange pass will re-measure this child via
   *      MeasureStandaloneChildren/ArrangeStandaloneChildren with the real
   *      size.
   *   3. Else (no parent actor): use the window-size constraint passed in.
   *
   * - WRAP_CONTENT / MATCH_PARENT: RequestedWidth/Height < 0 → fall through
   *   to parent/window path.
   * - Fixed (>= 0): takes precedence over parent/window.
   */
  void ProcessLayoutRoot(ViewImpl* view, float widthConstraint, float heightConstraint)
  {
    if(!view)
    {
      return;
    }

    float layoutWidth  = view->GetRequestedWidth();
    float layoutHeight = view->GetRequestedHeight();

    Actor self   = view->Self();
    Actor parent = self.GetParent();

    if(layoutWidth >= 0.0f)
    {
      widthConstraint = layoutWidth;
    }
    else if(parent)
    {
      widthConstraint = parent.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    }

    if(layoutHeight >= 0.0f)
    {
      heightConstraint = layoutHeight;
    }
    else if(parent)
    {
      heightConstraint = parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
    }

    // Root view has no parent that subtracts margin, so do it here. Margin,
    // requested position and min/max are stored in natural units, while the
    // layout pass works in visual (scale-applied) units. Convert with the
    // root's effective scale so the root matches the non-root path, which
    // already scales these properties.
    float   s        = view->GetEffectiveScale();
    Extents margin   = view->GetMargin();
    float   marginW  = static_cast<float>(margin.start + margin.end) * s;
    float   marginH  = static_cast<float>(margin.top + margin.bottom) * s;
    widthConstraint  = std::max(0.0f, widthConstraint - marginW);
    heightConstraint = std::max(0.0f, heightConstraint - marginH);

    // Measure pass
    MeasuredSize measuredSize = view->Measure(widthConstraint, heightConstraint);

    // Arrange pass: use the user-set position (parent is not a layout).
    // MATCH_PARENT roots fill the available constraint rather than using
    // their measured (minimum) size.
    LayoutRect bounds;
    bounds.x      = (view->GetRequestedPositionX() + static_cast<float>(margin.start)) * s;
    bounds.y      = (view->GetRequestedPositionY() + static_cast<float>(margin.top)) * s;
    bounds.width  = (layoutWidth == MATCH_PARENT) ? widthConstraint : measuredSize.width;
    bounds.height = (layoutHeight == MATCH_PARENT) ? heightConstraint : measuredSize.height;

    // Root has no parent layout to clamp against, so enforce the view's
    // own min/max here. For MATCH_PARENT axes, the measured value was
    // discarded above, so this is the only place min/max is applied.
    bounds.width  = std::min(std::max(bounds.width, view->GetMinimumWidth() * s), view->GetMaximumWidth() * s);
    bounds.height = std::min(std::max(bounds.height, view->GetMinimumHeight() * s), view->GetMaximumHeight() * s);

    view->Arrange(bounds);
  }

  /**
   * @brief Callback for window resize signal.
   */
  void OnWindowResized(Window window, Window::WindowSize size)
  {
    OnWindowResize(size.GetWidth(), size.GetHeight());
  }

private:
  Dali::WeakHandle<Window>                              mWindow;
  std::unique_ptr<Internal::LayoutTransitionDispatcher> mTransitionDispatcher;
  std::unordered_map<ViewImpl*, LayoutRootEntry>        mAllLayoutRoots; ///< All known layout roots (weak-referenced)
  std::unordered_set<ViewImpl*>                         mPendingViews;   ///< Dirty layout roots needing processing
  int32_t                                               mWindowWidth;
  int32_t                                               mWindowHeight;
  void*                                                 mWindowObjectPtr; ///< For self-destruct case.
  bool                                                  mProcessingScheduled;
};

} // namespace Integration

LayoutController& LayoutController::Get(Window window)
{
  DALI_ASSERT_ALWAYS(Adaptor::IsAvailable() && "LayoutController::Get() could not be called from worker thread, or app is not running!");

  void* key = window.GetObjectPtr();

  auto it = gLayoutControllers.find(key);
  if(it != gLayoutControllers.end())
  {
    auto& layoutController = *(it->second);
    if(DALI_UNLIKELY(layoutController.GetCurrentWindow() != window))
    {
      layoutController.ReplaceCurrentWindow(window);
    }
    return *(it->second);
  }

  // Create new controller for this window
  auto  controller        = std::unique_ptr<LayoutController>(new LayoutController(window));
  auto& ref               = *controller;
  gLayoutControllers[key] = std::move(controller);

  return ref;
}

void LayoutController::Remove(Window window)
{
  if(DALI_LIKELY(Adaptor::IsAvailable()))
  {
    if(window)
    {
      gLayoutControllers.erase(window.GetObjectPtr());
    }
  }
}

void LayoutController::UnregisterFromAll(ViewImpl* view)
{
  if(DALI_LIKELY(Adaptor::IsAvailable()))
  {
    for(auto& pair : gLayoutControllers)
    {
      pair.second->UnregisterView(view);
    }
  }
}

LayoutController::LayoutController(Window window)
: mImpl(MakeUnique<Integration::LayoutControllerImpl>(window))
{
}

LayoutController::~LayoutController()
{
}

void LayoutController::RequestLayout(ViewImpl* view)
{
  mImpl->RequestLayout(view);
}

void LayoutController::UnregisterView(ViewImpl* view)
{
  mImpl->UnregisterView(view);
}

void LayoutController::OnWindowResize(int32_t width, int32_t height)
{
  mImpl->OnWindowResize(width, height);
}

void LayoutController::ProcessLayouts()
{
  mImpl->ProcessLayouts();
}

void LayoutController::ScheduleLayoutExit(ViewImpl* parent, Ui::View child, ViewImpl* transitionOwner)
{
  mImpl->ScheduleLayoutExit(parent, child, transitionOwner);
}

void LayoutController::NotifyChildReparented(ViewImpl* child)
{
  mImpl->NotifyChildReparented(child);
}

void LayoutController::NotifyChildAdded(ViewImpl* directParent, Ui::View child)
{
  mImpl->NotifyChildAdded(directParent, child);
}

void LayoutController::ClearPendingInheritedEnters(ViewImpl* owner)
{
  mImpl->ClearPendingInheritedEnters(owner);
}

Dali::Window LayoutController::GetCurrentWindow() const
{
  return mImpl->GetCurrentWindow();
}

void LayoutController::ReplaceCurrentWindow(Dali::Window window)
{
  mImpl->ReplaceCurrentWindow(window);
}

} // namespace Ui
} // namespace Dali
