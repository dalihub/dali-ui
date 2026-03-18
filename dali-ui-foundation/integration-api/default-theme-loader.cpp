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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/default-theme-loader.h>

namespace
{

Dali::Vector4 HexToColor(uint32_t rgb, float a = 1.0f)
{
  return Dali::Vector4(
    static_cast<float>((rgb >> 16) & 0xFF) / 255.0f,
    static_cast<float>((rgb >> 8) & 0xFF) / 255.0f,
    static_cast<float>(rgb & 0xFF) / 255.0f,
    a);
}

} // unnamed namespace

namespace Dali
{
namespace Ui
{
namespace Integration
{

DefaultThemeLoader::DefaultThemeLoader()
{
  mColors["Primary"]    = HexToColor(0x387AFF);
  mColors["Background"] = HexToColor(0xF1F1F3);
  mColors["Outline"]    = HexToColor(0xB7B7BB);
}

DefaultThemeLoader::~DefaultThemeLoader() = default;

bool DefaultThemeLoader::GetColor(StringView colorId, Vector4& outColor)
{
  auto it = mColors.find(String(colorId));
  if(it != mColors.end())
  {
    outColor = it->second;
    return true;
  }
  return false;
}

String DefaultThemeLoader::GetCurrentThemeId() const
{
  return String("default");
}

DefaultThemeLoader::ThemeChangedSignalType& DefaultThemeLoader::ThemeChangedSignal()
{
  return mThemeChangedSignal;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
