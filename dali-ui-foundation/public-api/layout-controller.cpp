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
#include <dali-ui-foundation/public-api/layout-controller.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/processor-interface.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <algorithm>

namespace Dali
{
namespace UI
{
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
    BaseHandle handle;             ///< Ref-counted handle keeps the actor alive
    Integration::ViewImpl* view;   ///< Raw pointer for direct access
  };

  /**
   * @brief Constructor.
   */
  explicit LayoutControllerImpl(Window window)
    : mWindow(window),
      mWindowWidth(0),
      mWindowHeight(0),
      mProcessingScheduled(false)
  {
    // Get initial window size
    Vector2 size = window.GetSize();
    mWindowWidth = static_cast<int32_t>(size.width);
    mWindowHeight = static_cast<int32_t>(size.height);

    // Register as a processor with the adaptor (postProcess=false: run before dali Relayout)
    if (Adaptor::IsAvailable())
    {
      Adaptor::Get().RegisterProcessor(*this, false);
    }

    // Connect to window resize signal
    window.ResizeSignal().Connect(this, &LayoutControllerImpl::OnWindowResized);
  }

  /**
   * @brief Destructor.
   */
  ~LayoutControllerImpl() override
  {
    // Unregister from adaptor
    if (Adaptor::IsAvailable())
    {
      Adaptor::Get().UnregisterProcessor(*this);
    }
  }

  /**
   * @brief Schedules a view with layout capability for processing.
   */
  void RequestLayout(Integration::ViewImpl* view)
  {
    if (!view || !view->HasLayoutManager())
    {
      return;
    }

    // Track this layout root (ref-counted handle prevents dangling pointer)
    BaseHandle handle(view->Self());
    mAllLayoutRoots[view] = LayoutRootEntry{handle, view};

    // Add to pending (dirty) set
    mPendingViews.insert(view);

    // Schedule processing if not already scheduled
    if (!mProcessingScheduled)
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
   * @brief Called when window is resized.
   */
  void OnWindowResize(int32_t width, int32_t height)
  {
    mWindowWidth = width;
    mWindowHeight = height;

    // Invalidate ALL known layout roots (not just pending ones)
    for (auto& pair : mAllLayoutRoots)
    {
      if (pair.second.view)
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
    if (mPendingViews.empty())
    {
      return;
    }

    // Copy pending views and clear (in case new views are added during processing)
    std::vector<Integration::ViewImpl*> viewsToProcess(mPendingViews.begin(), mPendingViews.end());
    mPendingViews.clear();
    mProcessingScheduled = false;

    // Process each layout root
    for (auto* view : viewsToProcess)
    {
      // Verify the view is still tracked (not destroyed)
      if (view && mAllLayoutRoots.count(view) > 0)
      {
        ProcessLayoutRoot(view);
      }
    }
  }

  /**
   * @brief Implementation of Integration::Processor::Process.
   *
   * Called once per frame by DALi's adaptor.
   */
  void Process(bool postProcess) override
  {
    if (!postProcess)
    {
      ProcessLayouts();
    }
  }

  /**
   * @brief Implementation of Integration::Processor::GetProcessorName.
   */
  std::string_view GetProcessorName() const override
  {
    return "UI::LayoutController";
  }


private:
  /**
   * @brief Processes a single view with layout capability (layout root).
   *
   * Constraint for the root view:
   * - If the view's Actor has a parent (e.g. Layout under an Actor), use parent Actor's size.
   * - If no parent or parent size is zero, use window size.
   * - MatchParent: view gets the same size as the constraint.
   * - WrapContent: layout measures with constraint as maximum and returns wrapped size.
   * - Fixed (> 0): constraint for that dimension is the fixed value.
   */
  void ProcessLayoutRoot(Integration::ViewImpl* view)
  {
    if (!view)
    {
      return;
    }

    float layoutWidth = view->GetLayoutWidth();
    float layoutHeight = view->GetLayoutHeight();

    // Default: window size (when root is directly under window or parent size unknown).
    Vector2 windowSize = mWindow.GetSize();
    float widthConstraint = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.width)));
    float heightConstraint = static_cast<float>(std::max(0, static_cast<int32_t>(windowSize.height)));

    // If root view has a parent Actor (e.g. Actor -> Layout -> View), use parent's size as constraint.
    Actor self = view->Self();
    Actor parent = self.GetParent();
    if (parent)
    {
      float parentW = parent.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float parentH = parent.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      if (parentW > 0.0f && parentH > 0.0f)
      {
        widthConstraint = parentW;
        heightConstraint = parentH;
      }
    }

    if (layoutWidth > 0)
    {
      widthConstraint = layoutWidth;
    }
    if (layoutHeight > 0)
    {
      heightConstraint = layoutHeight;
    }

    // Measure pass
    MeasuredSize measuredSize = view->Measure(widthConstraint, heightConstraint);

    // Arrange pass: set view position and size (root at 0,0)
    LayoutRect bounds;
    bounds.x = 0.0f;
    bounds.y = 0.0f;
    bounds.width = measuredSize.width;
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
  Window mWindow;
  std::unordered_map<Integration::ViewImpl*, LayoutRootEntry> mAllLayoutRoots; ///< All known layout roots (ref-counted)
  std::unordered_set<Integration::ViewImpl*> mPendingViews;                    ///< Dirty layout roots needing processing
  int32_t mWindowWidth;
  int32_t mWindowHeight;
  bool mProcessingScheduled;
};

} // namespace Integration

namespace
{
// File-static map to store LayoutController instances per Window (internal use only; not exposed in public API).
std::unordered_map<void*, std::unique_ptr<LayoutController>> gLayoutControllers;
} // namespace

LayoutController& LayoutController::Get(Window window)
{
  void* key = window.GetObjectPtr();

  auto it = gLayoutControllers.find(key);
  if (it != gLayoutControllers.end())
  {
    return *(it->second);
  }

  // Create new controller for this window
  auto controller = std::unique_ptr<LayoutController>(new LayoutController(window));
  auto& ref = *controller;
  gLayoutControllers[key] = std::move(controller);

  return ref;
}

LayoutController::LayoutController(Window window)
  : mImpl(std::make_unique<Integration::LayoutControllerImpl>(window))
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

} // namespace UI
} // namespace Dali
