#ifndef DALI_UI_INTEGRATION_TEXT_ASYNC_TEXT_INTERFACE_H
#define DALI_UI_INTEGRATION_TEXT_ASYNC_TEXT_INTERFACE_H

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

namespace Dali
{
namespace Ui
{
namespace Text
{
struct AsyncTextRenderInfo;
} // namespace Text

namespace Integration
{
namespace Text
{
namespace Async
{
enum RequestType
{
  RENDER_FIXED_SIZE,
  RENDER_FIXED_WIDTH,
  RENDER_FIXED_HEIGHT,
  RENDER_CONSTRAINT,
  COMPUTE_NATURAL_SIZE,
  COMPUTE_HEIGHT_FOR_WIDTH,
};
} // namespace Async

/**
 * @brief An interface used by the text component which implement async text.
 */
class AsyncTextInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~AsyncTextInterface() = default;

  /**
   * @brief Initializes marquee using asynchronous text render information.
   *
   * @param[in] renderInfo The asynchronous text render information.
   */
  virtual void AsyncInitializeMarquee(const Ui::Text::AsyncTextRenderInfo& renderInfo) = 0;

  /**
   * @brief Called when the text fit point size has changed asynchronously.
   *
   * @param[in] pointSize The fitted point size.
   */
  virtual void AsyncTextFitChanged(float pointSize) = 0;

  /**
   * @brief Called when asynchronous text rendering has finished.
   *
   * @param[in] renderInfo The asynchronous text render information.
   */
  virtual void AsyncRenderFinished(Ui::Text::AsyncTextRenderInfo&& renderInfo) = 0;

  /**
   * @brief Called when asynchronous size computation has finished.
   *
   * @param[in] renderInfo The asynchronous text render information.
   */
  virtual void AsyncSizeComputed(const Ui::Text::AsyncTextRenderInfo& renderInfo) = 0;
};

} // namespace Text

} // namespace Integration

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTEGRATION_TEXT_ASYNC_TEXT_INTERFACE_H
