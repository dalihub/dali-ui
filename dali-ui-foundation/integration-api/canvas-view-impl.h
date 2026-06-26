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
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-drawable.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/canvas-view-rasterize-task.h>
#include <dali-ui-foundation/public-api/canvas-view-properties.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class CanvasViewImpl;
using CanvasViewImplPtr = IntrusivePtr<CanvasViewImpl>;

/**
 * @brief Internal implementation class for CanvasView.
 *
 * Manages a ThorVG CanvasRenderer, drives async/sync rasterization via the
 * Adaptor Processor interface, and registers the resulting texture as an
 * ImageVisual on the view.
 *
 * @see Dali::Ui::CanvasView
 */
class DALI_UI_API CanvasViewImpl : public ViewImpl, public Dali::Integration::Processor
{
public: // Properties
  /**
   * @brief Property indices aliased from the shared CanvasViewPropertyIndex.
   */
  struct Property
  {
    enum
    {
      VIEW_BOX                       = Ui::CanvasViewPropertyIndex::VIEW_BOX,
      SYNCHRONOUS_LOADING            = Ui::CanvasViewPropertyIndex::SYNCHRONOUS_LOADING,
      RASTERIZATION_REQUEST_MANUALLY = Ui::CanvasViewPropertyIndex::RASTERIZATION_REQUEST_MANUALLY,
      CANVAS_CONTENT_VISUAL          = Ui::CanvasViewPropertyIndex::CANVAS_CONTENT_VISUAL,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief Constructor.
   *
   * @param[in] viewBox Initial viewbox size.  Pass Vector2::ZERO if the viewbox
   *                    should match the view's layout size.
   */
  explicit CanvasViewImpl(const Vector2& viewBox);

  /**
   * @brief A reference-counted object may only be deleted by calling Unreference().
   */
  ~CanvasViewImpl() override;

public: // Creation
  /**
   * @brief Creates a new CanvasViewImpl.
   *
   * @param[in] viewBox Initial viewbox size
   * @return An intrusive pointer to the newly allocated CanvasViewImpl
   */
  static CanvasViewImplPtr New(const Vector2& viewBox);

public: // Property registration callbacks
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // Drawable management
  /**
   * @copydoc Dali::Ui::CanvasView::AddDrawable
   */
  bool AddDrawable(Dali::CanvasRenderer::Drawable& drawable);

  /**
   * @copydoc Dali::Ui::CanvasView::RemoveDrawable
   */
  bool RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable);

  /**
   * @copydoc Dali::Ui::CanvasView::RemoveAllDrawables
   */
  bool RemoveAllDrawables();

  /**
   * @copydoc Dali::Ui::CanvasView::SetDropShadow
   */
  void SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius);

  /**
   * @copydoc Dali::Ui::CanvasView::ClearDropShadow
   */
  void ClearDropShadow();

  /**
   * @copydoc Dali::Ui::CanvasView::HasDropShadow
   */
  bool HasDropShadow() const;

  /**
   * @copydoc Dali::Ui::CanvasView::SetGaussianBlur
   */
  void SetGaussianBlur(float blurRadius);

  /**
   * @copydoc Dali::Ui::CanvasView::ClearGaussianBlur
   */
  void ClearGaussianBlur();

  /**
   * @copydoc Dali::Ui::CanvasView::HasGaussianBlur
   */
  bool HasGaussianBlur() const;

  /**
   * @copydoc Dali::Ui::CanvasView::SetEffectAutoPaddingEnable
   */
  void SetEffectAutoPaddingEnable(bool enable);

  /**
   * @copydoc Dali::Ui::CanvasView::IsEffectAutoPaddingEnabled
   */
  bool IsEffectAutoPaddingEnabled() const;

public: // Rasterization control
  /**
   * @copydoc Dali::Ui::CanvasView::SetViewBox
   */
  void SetViewBox(const Vector2& viewBox);

  /**
   * @copydoc Dali::Ui::CanvasView::GetViewBox
   */
  Vector2 GetViewBox();

  /**
   * @copydoc Dali::Ui::CanvasView::SetSynchronousLoading
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @copydoc Dali::Ui::CanvasView::IsSynchronousLoading
   */
  bool IsSynchronousLoading() const;

  /**
   * @copydoc Dali::Ui::CanvasView::SetRasterizationRequestManually
   */
  void SetRasterizationRequestManually(bool manually);

  /**
   * @copydoc Dali::Ui::CanvasView::IsRasterizationRequestManually
   */
  bool IsRasterizationRequestManually() const;

  /**
   * @copydoc Dali::Ui::CanvasView::RequestRasterization
   */
  void RequestRasterization();

  /**
   * @brief Applies the rasterized image to the view as a content visual.
   *
   * Called from the main thread when an async rasterizing task completes,
   * or immediately after a synchronous rasterize.
   *
   * @param[in] task The completed rasterizing task
   */
  void ApplyRasterizedImage(CanvasViewRasterizingTaskPtr task);

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc CustomActorImpl::OnSceneConnection
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection
   */
  void OnSceneDisconnection() override;

private: // From Dali::Integration::Processor
  /**
   * @copydoc Dali::Integration::Processor::Process
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName
   */
  std::string_view GetProcessorName() const override
  {
    return "CanvasViewImpl";
  }

private:
  /**
   * @brief Schedules the next rasterization pass via the Adaptor processor queue.
   *
   * No-op if the processor is already registered or the Adaptor is unavailable.
   */
  void ScheduleRasterization();

  /**
   * @brief Creates and queues (or immediately executes) a rasterizing task.
   *
   * @param[in] forceProcess When true, skips the IsCanvasChanged() check and
   *                         always rasterizes (used on first display).
   */
  void AddRasterizationTask(bool forceProcess);

private:
  CanvasViewImpl(const CanvasViewImpl&)            = delete;
  CanvasViewImpl& operator=(const CanvasViewImpl&) = delete;

private:
  CanvasRenderer               mCanvasRenderer;
  Dali::Texture                mTexture;
  Dali::Ui::ImageUrl           mImageUrl;
  Vector2                      mSize;
  CanvasViewRasterizingTaskPtr mRasterizingTask;
  Ui::Visual::Base             mContentVisual;
  WeakHandle<Window>           mPlacementWindow;

  bool mIsSynchronous : 1;
  bool mManualRasterization : 1;
  bool mProcessorRegistered : 1;
  bool mLastCommitRasterized : 1;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
