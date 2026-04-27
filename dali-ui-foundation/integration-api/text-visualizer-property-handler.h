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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text-visualizer-impl.h>

namespace Dali::Ui::Integration
{
/**
 * Class to manage properties for the TextVisualizerImpl
 */
struct TextVisualizerImpl::PropertyHandler
{
  /**
   * Set properties on the text visualizer
   *
   * @param[in] view The handle for the text visualizer
   * @param[in] index The property index of the property to set
   * @param[in] value The value to set
   */
  static void SetProperty(Ui::View view, Property::Index index, const Property::Value& value);

  /**
   * Get properties from the text visualizer
   *
   * @param[in] view The handle for the text visualizer
   * @param[in] index The property index of the property to get
   * @return the value
   */
  static Property::Value GetProperty(Ui::View view, Property::Index index);
};

} // namespace Dali::Ui::Integration
