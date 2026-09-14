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

#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali-ui-foundation/internal/text/reveal/reveal-extension.h>
#include <algorithm>
#include <cmath>

namespace DALI_NAMESPACE::Ui::Integration::Text::Reveal
{
void SetBlurRadius(Ui::Text::Reveal& reveal, float radius)
{
  Ui::Text::Internal::Reveal::Extension::SetBlurRadius(reveal, std::isnan(radius) ? 0.0f : std::clamp(radius, 0.0f, 64.0f));
}

float GetBlurRadius(const Ui::Text::Reveal& reveal)
{
  return Ui::Text::Internal::Reveal::Extension::GetBlurRadius(reveal);
}

void SetBlurDurationRatio(Ui::Text::Reveal& reveal, float ratio)
{
  Ui::Text::Internal::Reveal::Extension::SetBlurDurationRatio(reveal, std::isnan(ratio) ? 0.0f : std::clamp(ratio, 0.0f, 1.0f));
}

float GetBlurDurationRatio(const Ui::Text::Reveal& reveal)
{
  return Ui::Text::Internal::Reveal::Extension::GetBlurDurationRatio(reveal);
}

void SetBlurMode(Ui::Text::Reveal& reveal, BlurMode mode)
{
  Ui::Text::Internal::Reveal::Extension::SetBlurMode(reveal, static_cast<uint8_t>(mode));
}

BlurMode GetBlurMode(const Ui::Text::Reveal& reveal)
{
  return static_cast<BlurMode>(Ui::Text::Internal::Reveal::Extension::GetBlurMode(reveal));
}
} //namespace DALI_NAMESPACE::Ui::Integration::Text::Reveal
