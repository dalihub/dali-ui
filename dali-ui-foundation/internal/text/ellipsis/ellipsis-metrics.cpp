/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/ellipsis/ellipsis-metrics.h>

namespace Dali::Ui::Text
{
bool ResolveFontClientEllipsisMetrics(TextAbstraction::FontClient& fontClient,
                                      FontId                       fontId,
                                      bool                         allowDefaultFont,
                                      TextAbstraction::GlyphInfo&  metrics)
{
  if(fontId == 0u && allowDefaultFont)
  {
    TextAbstraction::FontDescription defaultFontDescription;
    fontId = fontClient.GetFontId(defaultFontDescription,
                                  TextAbstraction::FontClient::DEFAULT_POINT_SIZE);
  }
  if(fontId == 0u)
  {
    return false;
  }

  metrics = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(fontId));
  return true;
}

} // namespace Dali::Ui::Text
