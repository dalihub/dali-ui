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
#include <dali/public-api/object/property-index-ranges.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for Dialog.
 *
 * Each dali-ui-components control reserves a distinct 1000-slot range starting
 * from a unique offset above Ui::VIEW_PROPERTY_END_INDEX to avoid collisions
 * (chart = +2001, navigator = +4001, dialog = +6001, dialog-container = +8001,
 *  alert-dialog = +10001).
 */
struct DialogPropertyIndex
{
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 6001,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };

  // Property enumerators are added as the Dialog API is implemented (P1+).
};

/**
 * @brief Property index definitions for DialogContainer.
 */
struct DialogContainerPropertyIndex
{
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 8001,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };
};

/**
 * @brief Property index definitions for AlertDialog.
 */
struct AlertDialogPropertyIndex
{
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 10001,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,
  };
};

} // namespace Ui
} // namespace Dali
