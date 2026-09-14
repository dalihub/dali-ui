#ifndef DALI_UI_INTERNAL_TEXT_REVEAL_EXTENSION_H
#define DALI_UI_INTERNAL_TEXT_REVEAL_EXTENSION_H

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

#include <dali/public-api/common/dali-common.h>
#include <cstdint>

namespace DALI_NAMESPACE::Ui::Text
{
class Reveal;

namespace Internal::Reveal
{
/**
 * @brief Accesses blur state without exposing the Reveal implementation.
 *
 * Integration callers normalize radius and duration before storing them.
 * Mode stores the authored byte, including unknown values; 0 is PERFORMANCE
 * and 1 is QUALITY. Rendering resolves unsupported values independently.
 * All operations reject None and moved-from values, like public accessors.
 */
struct Extension
{
  static void  SetBlurRadius(Ui::Text::Reveal& reveal, float radius);
  static float GetBlurRadius(const Ui::Text::Reveal& reveal);

  static void  SetBlurDurationRatio(Ui::Text::Reveal& reveal, float ratio);
  static float GetBlurDurationRatio(const Ui::Text::Reveal& reveal);

  static void    SetBlurMode(Ui::Text::Reveal& reveal, uint8_t mode);
  static uint8_t GetBlurMode(const Ui::Text::Reveal& reveal);
};
} // namespace Internal::Reveal
} //namespace DALI_NAMESPACE::Ui::Text

#endif // DALI_UI_INTERNAL_TEXT_REVEAL_EXTENSION_H
