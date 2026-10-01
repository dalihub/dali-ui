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
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief Public property indices for WidgetView and its implementation.
 */
struct WidgetViewPropertyIndex
{
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };

  enum
  {
    WIDGET_ID = PROPERTY_START_INDEX,
    INSTANCE_ID,
    CONTENT_INFO,
    TITLE,
    UPDATE_PERIOD,
    PREVIEW,
    WIDGET_STATE_FAULTED,
    PERMANENT_DELETE,
    KEEP_WIDGET_SIZE,
  };
};
} // namespace Ui
} // namespace DALI_NAMESPACE
