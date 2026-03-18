/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include "test-theme-loader.h"

#include <cstdlib>
#include <cstring>

using namespace Dali;

namespace
{
TestThemeLoader* gTestThemeLoader{nullptr};

struct ColorEntry
{
  const char* id;
  Vector4     rgba;
};

static constexpr ColorEntry BRIGHT_COLORS[] = {
  {"Primary", Vector4(0.10f, 0.45f, 1.00f, 1.0f)},
  {"Background", Vector4(0.98f, 0.98f, 0.99f, 1.0f)},
  {"Outline", Vector4(0.70f, 0.70f, 0.75f, 1.0f)},
  {"ThemeColor1", Vector4(1.0f, 0.0f, 0.0f, 1.0f)},
  {"ThemeColor2", Vector4(0.0f, 1.0f, 0.0f, 1.0f)},
  {"ThemeColor3", Vector4(0.0f, 0.0f, 1.0f, 1.0f)},
};

static constexpr ColorEntry DARK_COLORS[] = {
  {"Primary", Vector4(0.55f, 0.75f, 1.00f, 1.0f)},
  {"Background", Vector4(0.10f, 0.11f, 0.12f, 1.0f)},
  {"Outline", Vector4(0.35f, 0.35f, 0.40f, 1.0f)},
  {"ThemeColor1", Vector4(0.80f, 0.20f, 0.20f, 1.0f)},
  {"ThemeColor2", Vector4(0.20f, 0.80f, 0.20f, 1.0f)},
  {"ThemeColor3", Vector4(0.20f, 0.20f, 0.80f, 1.0f)},
};

inline bool FindColor(const ColorEntry* table, size_t tableSize, const char* key, Vector4& outColor)
{
  for(size_t i = 0; i < tableSize; ++i)
  {
    if(std::strcmp(table[i].id, key) == 0)
    {
      outColor = table[i].rgba;
      return true;
    }
  }
  return false;
}

inline bool FindBrightColor(const char* colorId, Vector4& outColor)
{
  return FindColor(BRIGHT_COLORS, sizeof(BRIGHT_COLORS) / sizeof(BRIGHT_COLORS[0]), colorId, outColor);
}

inline bool FindDarkColor(const char* colorId, Vector4& outColor)
{
  return FindColor(DARK_COLORS, sizeof(DARK_COLORS) / sizeof(DARK_COLORS[0]), colorId, outColor);
}

} // namespace

TestThemeLoader::TestThemeLoader()
  : mCurrentThemeId("bright")
{
}

TestThemeLoader::~TestThemeLoader()
{
  if(gTestThemeLoader == this)
  {
    gTestThemeLoader = nullptr;
  }
}

bool TestThemeLoader::GetColor(StringView colorId, Vector4& outColor)
{
  return mCurrentThemeId == "dark" ? FindDarkColor(colorId.Data(), outColor) : FindBrightColor(colorId.Data(), outColor);
}

String TestThemeLoader::GetCurrentThemeId() const
{
  return String(mCurrentThemeId.c_str());
}

TestThemeLoader::ThemeChangedSignalType& TestThemeLoader::ThemeChangedSignal()
{
  return mThemeChangedSignal;
}

void TestThemeLoader::ToggleTheme()
{
  mCurrentThemeId = (mCurrentThemeId == "dark") ? "bright" : "dark";
  mThemeChangedSignal.Emit();
}

TestThemeLoader* GetTestThemeLoader()
{
  return gTestThemeLoader;
}

void SetTestThemeLoader(TestThemeLoader* loader)
{
  gTestThemeLoader = loader;
}

