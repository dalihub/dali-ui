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

#include <dali-ui-test-suite-utils.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager.h>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

// Inspect ownership state without adding hooks to production code.
#define private public
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>
#undef private

using namespace Dali;

namespace
{
namespace UiText = Dali::Ui::Text;
}

int UtcDaliAsyncTextCustomFontRequestOrderP(void)
{
  UiTestApplication application;
  auto              manager = UiText::AsyncTextManager::Get();
  auto&             impl    = UiText::GetImplementation(manager);
  auto              running = impl.GetAvailableLoader();
  DALI_TEST_CHECK(running);

  // The same font path may be added repeatedly; keep the existing ordering
  // and duplicate semantics for both available and running loaders.
  impl.OnCustomFontAdded("first/font/path");
  impl.OnCustomFontAdded("second/font/path");
  impl.OnCustomFontAdded("first/font/path");

  std::vector<UiText::AsyncTextLoader> loaders = impl.mAvailableLoaders;
  loaders.push_back(running);
  for(auto& loader : loaders)
  {
    auto& state = UiText::GetImplementation(loader);
    DALI_TEST_EQUALS(state.mCustomFonts.size(), 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(state.mCustomFonts[0], std::string("first/font/path"), TEST_LOCATION);
    DALI_TEST_EQUALS(state.mCustomFonts[1], std::string("second/font/path"), TEST_LOCATION);
    DALI_TEST_EQUALS(state.mCustomFonts[2], std::string("first/font/path"), TEST_LOCATION);
  }

  // Returning/reassigning a loader does not consume or duplicate its requests.
  impl.ReleaseLoader(nullptr, running);
  auto reused = impl.GetAvailableLoader();
  DALI_TEST_CHECK(reused == running);
  DALI_TEST_EQUALS(UiText::GetImplementation(reused).mCustomFonts.size(), 3u, TEST_LOCATION);
  impl.ReleaseLoader(nullptr, reused);
  END_TEST;
}

int UtcDaliAsyncTextLocalePendingCustomFontP(void)
{
  UiTestApplication application;
  auto              manager = UiText::AsyncTextManager::Get();
  auto&             impl    = UiText::GetImplementation(manager);
  // Fix the initial manager locale so the first transition is independent of
  // the host locale. This changes loader state, not the process or OS locale.
  impl.OnLocaleChanged("en_US.UTF-8");
  auto loader = impl.GetAvailableLoader();
  DALI_TEST_CHECK(loader);

  // A running loader becomes locale-pending when its worker returns it.
  impl.OnLocaleChanged("ar_SA.UTF-8");
  impl.ReleaseLoader(nullptr, loader);
  DALI_TEST_EQUALS(impl.mLocaleChangedLoaders.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(impl.mLocaleChangedLoaders.front() == loader);

  impl.OnCustomFontAdded("first/font/path");
  impl.OnCustomFontAdded("second/font/path");
  auto& state = UiText::GetImplementation(loader);
  DALI_TEST_EQUALS(state.mCustomFonts.size(), 2u, TEST_LOCATION);

  // ClearModule and SetLocale must retain the queue for the next Initialize.
  impl.OnLocaleChanged("he_IL.UTF-8");
  impl.OnLocaleChanged("en_US.UTF-8");
  DALI_TEST_CHECK(impl.mLocaleChangedLoaders.empty());
  DALI_TEST_CHECK(impl.mRunningLoaders.empty());
  DALI_TEST_EQUALS(state.mLocale, std::string("en_US.UTF-8"), TEST_LOCATION);
  DALI_TEST_CHECK(!loader.IsModuleClearNeeded());
  DALI_TEST_CHECK(!loader.IsLocaleUpdateNeeded());
  DALI_TEST_EQUALS(state.mCustomFonts.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(state.mCustomFonts[0], std::string("first/font/path"), TEST_LOCATION);
  DALI_TEST_EQUALS(state.mCustomFonts[1], std::string("second/font/path"), TEST_LOCATION);
  END_TEST;
}
