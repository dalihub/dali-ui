#ifndef DALI_UI_INTEGRATION_TEXT_CONTROL_INTERFACE_H
#define DALI_UI_INTEGRATION_TEXT_CONTROL_INTERFACE_H

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace Text
{
/**
 * @brief An interface that the Text::Controller uses to request a text relayout.
 */
class DALI_UI_API ControlInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~ControlInterface()
  {
  }

  /**
   * @brief Called to request a text relayout.
   */
  virtual void RequestTextRelayout() = 0;

  /**
   * @brief Invalidate measured size.
   */
  virtual void InvalidateTextMeasure() = 0;

  /**
   * @brief Requests asynchronous text rendering.
   */
  virtual void RequestAsyncRender() = 0;
};

} // namespace Text

} // namespace Integration

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTEGRATION_TEXT_CONTROL_INTERFACE_H
