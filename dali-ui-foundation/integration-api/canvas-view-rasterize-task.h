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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/async-task-manager.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class CanvasViewRasterizingTask;
using CanvasViewRasterizingTaskPtr = IntrusivePtr<CanvasViewRasterizingTask>;

/**
 * @brief Async task that rasterizes a CanvasRenderer on a worker thread.
 *
 * Life cycle:
 *  1. Created by CanvasViewImpl on the main thread.
 *  2. Queued in AsyncTaskManager; processed on a worker thread.
 *  3. On completion, the registered callback is invoked on the main thread,
 *     allowing CanvasViewImpl to apply the resulting texture.
 *  4. If removed before processing, destroyed on the worker thread.
 */
class DALI_UI_API CanvasViewRasterizingTask : public AsyncTask
{
public:
  /**
   * @brief Constructor.
   *
   * @param[in] canvasRenderer The CanvasRenderer to rasterize.
   * @param[in] callback       Called on the main thread when rasterization completes.
   *                           CanvasViewImpl owns the pointed-to object; it must remain
   *                           alive for the duration of the task.
   */
  CanvasViewRasterizingTask(CanvasRenderer canvasRenderer, CallbackBase* callback);

  /**
   * @brief Destructor.
   */
  ~CanvasViewRasterizingTask() = default;

  /**
   * @brief Returns true if rasterization completed successfully.
   */
  bool IsRasterized() const;

  /**
   * @brief Returns the rasterized texture, or an empty handle if not yet rasterized.
   */
  Texture GetRasterizedTexture();

public: // AsyncTask interface
  /**
   * @copydoc Dali::AsyncTask::Process()
   */
  void Process() override;

  /**
   * @copydoc Dali::AsyncTask::IsReady()
   */
  bool IsReady() override;

  /**
   * @copydoc Dali::AsyncTask::GetTaskName()
   */
  Dali::StringView GetTaskName() const override
  {
    return "CanvasViewRasterizingTask";
  }

private:
  CanvasViewRasterizingTask(const CanvasViewRasterizingTask&)            = delete;
  CanvasViewRasterizingTask& operator=(const CanvasViewRasterizingTask&) = delete;

private:
  CanvasRenderer mCanvasRenderer;
  bool           mRasterizedSucceeded;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
