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
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
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
   * Stores both a ref-counted handle (to prevent dangling) and the raw
   * implementation pointer (for fast access during layout passes).
   */
  struct LayoutRootEntry
  {
    BaseHandle             handle; ///< Ref-counted handle keeps the actor alive
    Integration::ViewImpl* view;   ///< Raw pointer for direct access
  };

  /**
   * @brief Constructor.
   */
  explicit LayoutControllerImpl(Window window)
  : mWindow(window),
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
  void RequestLayout(Integration::ViewImpl* view)
  {
    if(!view)
    {
      return;
    }

    // Track this layout root (ref-counted handle prevents dangling pointer)
    BaseHandle handle(view->Self());
    mAllLayoutRoots[view] = LayoutRootEntry{handle, view};

    // Add to pending (dirty) set
    mPendingViews.insert(view);

    // Schedule processing if not already scheduled
    if(!mProcessingScheduled)
    {
      mProcessingScheduled = true;
    }
  }

  /**
   * @brief Removes a view from tracking (called when view is destroyed).
   */
  void UnregisterView(Integration::ViewImpl* view)
  {
    mAllLayoutRoots.erase(view);
    mPendingViews.erase(view);
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

    // Invalidate ALL known layout roots (not just pending ones)
    for(auto& pair : mAllLayoutRoots)
    {
      if(pair.second.view)
      {
        pair.second.view->InvalidateMeasure();
      }
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
    if(mPendingViews.empty())
    {
      return;
    }

    // Copy pending views and clear (in case new views are added during processing)
    decltype(mPendingViews) viewsToProcess;
    viewsToProcess.swap(mPendingViews);
    mProcessingScheduled = false;

    // Default: window size (when root is directly under window or parent size unknown).
    Vector2 windowSize       = window.GetSize();
    float   widthConstraint  = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.width)));
    float   heightConstraint = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.height)));

    // Process each layout root
    for(auto* view : viewsToProcess)
    {
      // Verify the view is still tracked (not destroyed)
      if(view && mAllLayoutRoots.count(view) > 0)
      {
        ProcessLayoutRoot(view, widthConstraint, heightConstraint);
      }
    }
  }

  /**
   * @brief Processes a single view with layout capability (layout root).
   *
   * Constraint for the root view:
   * - If the view's Actor has a parent (e.g. Layout under an Actor), use parent Actor's size.
   * - If no parent or parent size is zero, use window size.
   * - WRAP_CONTENT: layout measures with constraint as maximum and returns wrapped size.
   * - Fixed (> 0): constraint for that dimension is the fixed value.
   */
  void ProcessLayoutRoot(Integration::ViewImpl* view, float widthConstraint, float heightConstraint)
  {
    if(!view)
    {
      return;
    }

    float layoutWidth  = view->GetRequestedWidth();
    float layoutHeight = view->GetRequestedHeight();

    // If root view has a parent Actor (e.g. Actor -> Layout -> View), use parent's size as constraint.
    Actor self   = view->Self();
    Actor parent = self.GetParent();
    if(parent)
    {
      float parentW = parent.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float parentH = parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      if(parentW > 0.0f && parentH > 0.0f)
      {
        widthConstraint  = parentW;
        heightConstraint = parentH;
      }
    }

    if(layoutWidth > 0)
    {
      widthConstraint = layoutWidth;
    }
    if(layoutHeight > 0)
    {
      heightConstraint = layoutHeight;
    }

    // Measure pass
    MeasuredSize measuredSize = view->Measure(widthConstraint, heightConstraint);

    // Arrange pass: use the user-set position (parent is not a layout)
    LayoutRect bounds;
    bounds.x      = view->GetPositionX();
    bounds.y      = view->GetPositionY();
    bounds.width  = measuredSize.width;
    bounds.height = measuredSize.height;

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
  Dali::WeakHandle<Window>                                    mWindow;
  std::unordered_map<Integration::ViewImpl*, LayoutRootEntry> mAllLayoutRoots; ///< All known layout roots (ref-counted)
  std::unordered_set<Integration::ViewImpl*>                  mPendingViews;   ///< Dirty layout roots needing processing
  int32_t                                                     mWindowWidth;
  int32_t                                                     mWindowHeight;
  void*                                                       mWindowObjectPtr; ///< For self-destruct case.
  bool                                                        mProcessingScheduled;
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

void LayoutController::UnregisterFromAll(Integration::ViewImpl* view)
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

void LayoutController::RequestLayout(Integration::ViewImpl* view)
{
  mImpl->RequestLayout(view);
}

void LayoutController::UnregisterView(Integration::ViewImpl* view)
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
