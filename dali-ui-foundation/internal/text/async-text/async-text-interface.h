#ifndef DALI_UI_TEXT_ASYNC_TEXT_INTERFACE_H
#define DALI_UI_TEXT_ASYNC_TEXT_INTERFACE_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>

namespace Dali
{
class Actor;

namespace Ui
{
namespace Text
{
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
  virtual void AsyncInitializeMarquee(const Text::AsyncTextRenderInfo& renderInfo) = 0;

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
  virtual void AsyncRenderFinished(Text::AsyncTextRenderInfo&& renderInfo) = 0;

  /**
   * @brief Called when asynchronous size computation has finished.
   *
   * @param[in] renderInfo The asynchronous text render information.
   */
  virtual void AsyncSizeComputed(const Text::AsyncTextRenderInfo& renderInfo) = 0;
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ASYNC_TEXT_INTERFACE_H
