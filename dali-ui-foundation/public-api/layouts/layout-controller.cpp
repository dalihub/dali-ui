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
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{

namespace
{
// File-static map to store LayoutController instances per Window (internal use only; not exposed in public API).
std::unordered_map<void*, std::unique_ptr<LayoutController>> gLayoutControllers;

// Innermost LayoutControllerImpl whose ProcessLayoutRoot is on the stack, so
// ViewImpl::Arrange routes arranged subscribers in O(1). Managed by RAII
// ActiveLayoutFinishedScope (exception-safe: DALI_ASSERT_ALWAYS throws).
Integration::LayoutControllerImpl* gActiveLayoutFinishedController{nullptr};
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
   * @brief One queued View layout-finished event. @c view is the raw key (nulled
   * as a tombstone by UnregisterView); @c weakHandle is the safe emit handle.
   */
  struct PendingViewLayoutFinishedEvent
  {
    ViewImpl*                  view{nullptr};
    Dali::WeakHandle<Ui::View> weakHandle;
    LayoutRect                 bounds;
  };

  /**
   * @brief One active per-root collector frame (stack-local containers owned by
   * the ProcessLayouts loop). A member STACK of these makes nested ProcessLayouts
   * re-entrancy safe: UnregisterView scrubs a destroyed view from EVERY frame,
   * not just the innermost, closing the nested-rebind use-after-free.
   */
  struct ActiveCollectorFrame
  {
    ViewImpl*                      root;
    std::vector<ViewImpl*>*        views;
    std::unordered_set<ViewImpl*>* set;
  };

  /**
   * @brief RAII: push a collector frame + set the file-static pointer on entry;
   * pop + restore on exit. RAII (not manual save/restore) is REQUIRED because a
   * DALI_ASSERT_ALWAYS inside the arrange call stack throws Dali::DaliException
   * and there is no try/catch here; the destructor still pops/restores on unwind.
   */
  struct ActiveLayoutFinishedScope
  {
    ActiveLayoutFinishedScope(LayoutControllerImpl&          self,
                              ViewImpl*                      root,
                              std::vector<ViewImpl*>&        views,
                              std::unordered_set<ViewImpl*>& set)
    : mSelf(self),
      mPrevController(gActiveLayoutFinishedController)
    {
      mSelf.mActiveCollectorStack.push_back(ActiveCollectorFrame{root, &views, &set});
      gActiveLayoutFinishedController = &self;
    }
    ~ActiveLayoutFinishedScope()
    {
      gActiveLayoutFinishedController = mPrevController;
      mSelf.mActiveCollectorStack.pop_back();
    }
    ActiveLayoutFinishedScope(const ActiveLayoutFinishedScope&)            = delete;
    ActiveLayoutFinishedScope& operator=(const ActiveLayoutFinishedScope&) = delete;

    LayoutControllerImpl& mSelf;
    LayoutControllerImpl* mPrevController;
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
    auto    posSize = window.GetPositionSize();
    Vector2 size    = Vector2(posSize.width, posSize.height);
    mWindowWidth    = static_cast<int32_t>(size.width);
    mWindowHeight   = static_cast<int32_t>(size.height);

    // Register as a processor with the adaptor (postProcess=false: run before dali Relayout)
    if(DALI_LIKELY(Adaptor::IsAvailable()))
    {
      Adaptor::Get().RegisterProcessor(*this, false);

      // Connect to window resize signal
      window.ResizedSignal().Connect(this, &LayoutControllerImpl::OnWindowResized);
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

    // Real layout work has been requested since the last LayoutFinished emit;
    // arm the settled latch so the next drain-to-empty fires the signal.
    mLayoutDirtySinceEmit = true;

    // Schedule processing if not already scheduled
    if(!mProcessingScheduled)
    {
      mProcessingScheduled = true;

      // Guarantee a layout pass runs even if the app would otherwise idle, so
      // the pending work drains and LayoutFinished can eventually fire.
      if(DALI_LIKELY(Adaptor::IsAvailable()))
      {
        Adaptor::Get().RequestProcessEventsOnIdle();
      }
    }
  }

  /**
   * @brief Schedules an EXIT-slot transition (forwarded from ViewImpl::Remove).
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

    // Scrub EVERY active collector frame (nested passes each have their own),
    // closing the nested-rebind use-after-free: a view collected in an OUTER
    // frame and destroyed during an INNER pass must not remain a dangling raw
    // pointer that the outer snapshot dereferences.
    for(ActiveCollectorFrame& frame : mActiveCollectorStack)
    {
      frame.set->erase(view);
      frame.views->erase(std::remove(frame.views->begin(), frame.views->end(), view), frame.views->end());
    }
    // Scrub episode events: erase the index and tombstone matching slots (never a
    // mid-vector erase, which would shift every other index).
    mPendingViewLayoutFinishedEventIndex.erase(view);
    for(auto& event : mPendingViewLayoutFinishedEvents)
    {
      if(event.view == view)
      {
        event.view = nullptr;
      }
    }

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
      window.ResizedSignal().Connect(this, &LayoutControllerImpl::OnWindowResized);
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

    // A resize forces every root to recompute; ensure a layout pass runs even
    // if no other event wakes the event loop, so the invalidated roots drain
    // and LayoutFinished fires. Unconditional because InvalidateMeasure above
    // may early-exit for already-dirty roots without re-scheduling a pass.
    if(DALI_LIKELY(Adaptor::IsAvailable()))
    {
      Adaptor::Get().RequestProcessEventsOnIdle();
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
    ++mProcessDepth;

    Dali::Window window = mWindow.GetHandle();
    if(DALI_UNLIKELY(!window))
    {
      // The window is gone. Defer self-destruct to the outermost Process frame
      // (handled below) so a re-entrant Process/emit frame is never left
      // running on a destroyed controller.
      mDestroyPending = true;
    }
    else if(!postProcess)
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

      // "Fully settled" is evaluated only by the outermost Process frame
      // (mProcessDepth == 1) and AFTER TickAnimators, so any same-frame
      // re-dirty from an OnFinished / EXIT-Remove callback is already visible
      // in mPendingViews before we decide.
      if(mProcessDepth == 1)
      {
        if(mPendingViews.empty() && mLayoutDirtySinceEmit)
        {
          // Deliver every subscribed View's layout-finished event FIRST (in
          // traversal order), then decide the window signal.
          //
          // CAVEAT (do not "fix" by adding a cap without agreement): a View
          // LayoutFinishedSignal slot that unconditionally re-invalidates layout
          // spins an endless dirty->settled->emit cycle. This is intentionally
          // NOT capped here (consistent with the window signal and other
          // toolkits); the contract is documented on View::LayoutFinishedSignal
          // as "guard re-layout in the slot behind a real condition". A view is
          // also re-collected/re-emitted whenever it is re-arranged, even with
          // unchanged bounds, so callers must not treat an emit as "bounds
          // changed".
          EmitPendingViewLayoutFinishedSignals();

          if(mDestroyPending)
          {
            // A slot called Remove(window) -> deferred destroy at the outermost
            // Process() unwind. Skip BOTH the window Emit and the idle request.
          }
          else if(mPendingViews.empty() && mPendingViewLayoutFinishedEvents.empty())
          {
            // Truly settled: no re-queued layout work AND no View events stranded
            // by a nested depth>=2 ProcessLayouts (its settle gate was skipped).
            mLayoutDirtySinceEmit = false;
            mLayoutFinishedSignal.Emit(window);
          }
          else
          {
            // A slot re-invalidated, or a nested pass repopulated the events map.
            // Keep the latch armed and schedule a follow-up settled pass to drain.
            mLayoutDirtySinceEmit = true;
            if(DALI_LIKELY(Adaptor::IsAvailable()))
            {
              Adaptor::Get().RequestProcessEventsOnIdle();
            }
          }
        }
        else if(!mPendingViews.empty())
        {
          // Work was re-scheduled during this pass; not settled yet. Ensure a
          // follow-up pass runs even if the app would otherwise idle.
          mLayoutDirtySinceEmit = true;
          if(DALI_LIKELY(Adaptor::IsAvailable()))
          {
            Adaptor::Get().RequestProcessEventsOnIdle();
          }
        }
      }
    }

    // Execute any deferred self-destruct only when unwinding the outermost
    // Process frame, where no nested Process/emit frame remains on the stack.
    if(--mProcessDepth == 0 && mDestroyPending)
    {
      gLayoutControllers.erase(mWindowObjectPtr);
      // *this is destroyed here; do not access any member below.
    }
  }

  /**
   * @brief Implementation of Integration::Processor::GetProcessorName.
   */
  std::string_view GetProcessorName() const override
  {
    return "Ui::LayoutController";
  }

  /**
   * @brief Returns the signal emitted when this window's layout calculation
   * has fully settled (all Measure/Arrange work drained).
   */
  LayoutController::LayoutFinishedSignalType& LayoutFinishedSignal()
  {
    return mLayoutFinishedSignal;
  }

  /**
   * @brief Whether a Process() call is currently on the stack for this
   * controller. Used to defer a self-destruct requested from a slot.
   */
  bool IsProcessing() const
  {
    return mProcessDepth > 0;
  }

  /**
   * @brief Requests deferred self-destruct; the erase is performed when the
   * outermost Process() frame unwinds.
   */
  void RequestDestroy()
  {
    mDestroyPending = true;
  }

  /**
   * @brief Whether @p view is inside @p root's actor subtree. Null-safe off-scene:
   * a null root actor / GetParent()-empty ends the walk.
   */
  bool IsWithinActiveLayoutFinishedRoot(ViewImpl* root, ViewImpl* view) const
  {
    if(!root || !view)
    {
      return false;
    }
    Actor rootActor = root->Self();
    void* rootPtr   = rootActor ? rootActor.GetObjectPtr() : nullptr;
    if(!rootPtr)
    {
      return false;
    }
    for(Actor actor = view->Self(); actor; actor = actor.GetParent())
    {
      if(actor.GetObjectPtr() == rootPtr)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Records @p view as a candidate for the innermost (top) active root.
   * MUST be public: the free-function forwarder calls it on a LayoutControllerImpl*.
   */
  void NotifyViewArranged(ViewImpl* view)
  {
    if(mActiveCollectorStack.empty() || !view)
    {
      return;
    }
    ActiveCollectorFrame& top = mActiveCollectorStack.back();
    if(!IsWithinActiveLayoutFinishedRoot(top.root, view))
    {
      return;
    }
    if(top.set->insert(view).second)
    {
      top.views->push_back(view);
    }
  }

  /**
   * @brief Snapshots each arranged subscribed View's final (post-RTL, pre-
   * transition) actor bounds into the episode store (latest-wins, order-
   * preserving). Runs after ProcessLayoutRoot returns, before StartTransitions.
   */
  void SnapshotViewLayoutFinishedCandidates(const std::vector<ViewImpl*>& arrangedViews)
  {
    for(ViewImpl* arranged : arrangedViews)
    {
      if(!arranged)
      {
        continue;
      }
      Ui::View viewHandle = Ui::View::DownCast(arranged->Self());
      if(!viewHandle)
      {
        continue;
      }
      ViewImpl& arrangedImpl = GetImpl(viewHandle);
      if(!arrangedImpl.HasLayoutFinishedSignalConnections())
      {
        continue;
      }
      Actor      actor = arrangedImpl.Self();
      LayoutRect bounds(
        actor.GetProperty<float>(Actor::Property::POSITION_X),
        actor.GetProperty<float>(Actor::Property::POSITION_Y),
        actor.GetProperty<float>(Actor::Property::SIZE_WIDTH),
        actor.GetProperty<float>(Actor::Property::SIZE_HEIGHT));

      auto indexIt = mPendingViewLayoutFinishedEventIndex.find(arranged);
      if(indexIt == mPendingViewLayoutFinishedEventIndex.end())
      {
        std::size_t index                              = mPendingViewLayoutFinishedEvents.size();
        mPendingViewLayoutFinishedEventIndex[arranged] = index;
        mPendingViewLayoutFinishedEvents.push_back(
          PendingViewLayoutFinishedEvent{arranged, Dali::WeakHandle<Ui::View>(viewHandle), bounds});
      }
      else
      {
        PendingViewLayoutFinishedEvent& event = mPendingViewLayoutFinishedEvents[indexIt->second];
        event.weakHandle                      = Dali::WeakHandle<Ui::View>(viewHandle);
        event.bounds                          = bounds;
      }
    }
  }

  /**
   * @brief Emits all pending View events in traversal order, then clears the
   * episode store. Move-out first so a slot re-entering Process / mutating views
   * cannot corrupt iteration. Per entry: tombstone -> WeakHandle revive -> b1
   * stale-skip -> connection check -> emit. NO requeue, NO mid-loop early-return.
   */
  void EmitPendingViewLayoutFinishedSignals()
  {
    auto localEvents = std::move(mPendingViewLayoutFinishedEvents);
    mPendingViewLayoutFinishedEvents.clear();
    mPendingViewLayoutFinishedEventIndex.clear();
    for(auto& event : localEvents)
    {
      if(!event.view)
      {
        continue; // tombstone: destroyed BEFORE the move
      }
      Ui::View view = event.weakHandle.GetHandle();
      if(!view)
      {
        continue; // destroyed DURING emit -> no UAF (raw event.view not dereferenced)
      }
      ViewImpl& viewImpl = GetImpl(view);
      // b1 stale-skip: a nested ProcessLayouts during THIS emit re-queued a NEWER
      // member event for this view (the member store was cleared at move-out).
      // Skip the stale local event; the newer member event drains on the
      // follow-up settled pass, so the view emits once with its latest bounds.
      if(mPendingViewLayoutFinishedEventIndex.find(&viewImpl) != mPendingViewLayoutFinishedEventIndex.end())
      {
        continue;
      }
      if(!viewImpl.HasLayoutFinishedSignalConnections())
      {
        continue; // unsubscribed since snapshot
      }
      viewImpl.EmitLayoutFinishedSignal(event.bounds);
    }
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
      // Nothing to process: clear the scheduled flag so a later RequestLayout
      // re-arms the idle wakeup (RequestProcessEventsOnIdle). Without this the
      // flag can latch true (e.g. a queued root removed via UnregisterView
      // before its pass), suppressing the wakeup the LayoutFinishedSignal
      // relies on to fire in an otherwise-idle application.
      mProcessingScheduled = false;

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
    auto    posSize          = window.GetPositionSize();
    Vector2 windowSize       = Vector2(posSize.width, posSize.height);
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

        // Collect subscribed Views arranged under this root into STACK-LOCAL
        // containers; the RAII scope pushes a frame (and sets the file-static)
        // for the duration of ProcessLayoutRoot, popping on exit even if an
        // assert throws mid-arrange.
        std::vector<ViewImpl*>        arrangedViews;
        std::unordered_set<ViewImpl*> arrangedSet;
        {
          ActiveLayoutFinishedScope scope(*this, view, arrangedViews, arrangedSet);
          ProcessLayoutRoot(view, widthConstraint, heightConstraint);
        }

        // Snapshot now: ProcessLayoutRoot has applied RTL (actor POSITION_X final)
        // and StartTransitions has not yet overwritten actor props. Views destroyed
        // during ProcessLayoutRoot were scrubbed from arrangedViews by UnregisterView
        // (across all frames), and only property reads run here, so Self() is safe.
        SnapshotViewLayoutFinishedCandidates(arrangedViews);

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
  LayoutController::LayoutFinishedSignalType            mLayoutFinishedSignal;                ///< Emitted when layout fully settles for this window
  bool                                                  mLayoutDirtySinceEmit{false};         ///< Settled latch: armed by RequestLayout, cleared at emit
  int                                                   mProcessDepth{0};                     ///< Process() re-entrancy depth (emit gate + deferred-destroy safe point)
  bool                                                  mDestroyPending{false};               ///< Deferred self-destruct requested during processing
  std::vector<ActiveCollectorFrame>                     mActiveCollectorStack;                ///< Stack of per-root collectors (nested-pass safe)
  std::vector<PendingViewLayoutFinishedEvent>           mPendingViewLayoutFinishedEvents;     ///< Episode events (traversal order, latest-wins)
  std::unordered_map<ViewImpl*, std::size_t>            mPendingViewLayoutFinishedEventIndex; ///< view -> index into the events vector
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
      auto it = gLayoutControllers.find(window.GetObjectPtr());
      if(it != gLayoutControllers.end())
      {
        if(it->second->mImpl->IsProcessing())
        {
          // Called from within a Process()/LayoutFinishedSignal emit for this
          // controller; defer the erase until the outermost Process() frame
          // unwinds so we never destroy a controller that is on the stack.
          it->second->mImpl->RequestDestroy();
        }
        else
        {
          gLayoutControllers.erase(it);
        }
      }
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

void LayoutController::NotifyViewArranged(ViewImpl* view)
{
  if(gActiveLayoutFinishedController)
  {
    gActiveLayoutFinishedController->NotifyViewArranged(view);
  }
}

LayoutController::LayoutFinishedSignalType& LayoutController::LayoutFinishedSignal()
{
  return mImpl->LayoutFinishedSignal();
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
