#ifndef DALI_UI_TEXT_VISUALIZER_TEXT_PREPARER_H
#define DALI_UI_TEXT_VISUALIZER_TEXT_PREPARER_H

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
#include <dali-ui-foundation/internal/text/text-visualizer/prepared-text.h>

namespace Dali::Ui::Internal::TextVisualizer
{
/**
 * @brief Converts TextVisualizer state into a minimal PreparedText cache.
 */
class TextPreparer
{
public:
  struct Input
  {
    Dali::String text;
    Dali::String fontFamily;
    // TextVisualizer public FontSize is defined in pixels.
    // Prepare converts this pixel size into the internal point-size representation
    // required by the shaping/font client path.
    float fontSize{0.0f};
  };

public:
  static PreparedText Prepare(const Input& input);
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_TEXT_PREPARER_H
