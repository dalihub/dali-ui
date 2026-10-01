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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace WidgetView
{
/**
 * @brief Property indices for framework-level WidgetView access.
 *
 * Application code uses the typed WidgetView methods for text and shader settings.
 */
namespace Property
{
enum Type
{
  PROPERTY_START_INDEX = Dali::Ui::WidgetViewPropertyIndex::PROPERTY_START_INDEX,

  WIDGET_ID            = Dali::Ui::WidgetView::Property::WIDGET_ID,
  INSTANCE_ID          = Dali::Ui::WidgetView::Property::INSTANCE_ID,
  CONTENT_INFO         = Dali::Ui::WidgetView::Property::CONTENT_INFO,
  TITLE                = Dali::Ui::WidgetView::Property::TITLE,
  UPDATE_PERIOD        = Dali::Ui::WidgetView::Property::UPDATE_PERIOD,
  PREVIEW              = Dali::Ui::WidgetView::Property::PREVIEW,
  WIDGET_STATE_FAULTED = Dali::Ui::WidgetView::Property::WIDGET_STATE_FAULTED,
  PERMANENT_DELETE     = Dali::Ui::WidgetView::Property::PERMANENT_DELETE,
  KEEP_WIDGET_SIZE     = Dali::Ui::WidgetView::Property::KEEP_WIDGET_SIZE,

  /**
   * @brief Loading label settings: a visibility boolean or a map containing
   * stateText, fontStyle, textPixelSize and textColor. Reading returns visibility.
   */
  LOADING_TEXT = KEEP_WIDGET_SIZE + 1,

  /**
   * @brief Write-only retry label map. Accepts loading text settings and textVisible.
   */
  RETRY_TEXT,

  /**
   * @brief Write-only effect map containing a shader map. An empty map clears it.
   */
  EFFECT,
};
} // namespace Property
} // namespace WidgetView
} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
