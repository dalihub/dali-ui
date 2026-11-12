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

#include <dali/devel-api/adaptor-framework/web-engine/web-engine.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <string>
#include <vector>

namespace Test
{
namespace WebEngine
{
struct RuntimeState
{
  int32_t type = -1;
  bool started = false;
  bool available = false;
  bool failInitialization = false;
  bool failProfile = false;
  bool failView = false;
  bool backendUnavailable = false;
  bool lastIncognito = false;
  std::string lastBundlePath;
  std::vector<std::string> arguments;
  std::vector<std::string> initializedArguments;
  int initializationCalls = 0;
  int profileCreates = 0;
  int profileReleases = 0;
  int viewCreates = 0;
  int cookieClearCalls = 0;
  int cacheClearCalls = 0;
  const void* lastViewContext = nullptr;
};
RuntimeState& GetRuntimeState();
void TerminateRuntime();

void SetNativeImageInterface(Dali::WebEngine engine, Dali::NativeImageInterfacePtr nativeImage);
void SetWebViewAccessible(Dali::Accessibility::Accessible* webAccessible);
void SetWebAccessibleActivatedAddress(Dali::Devel::Accessibility::Address address);
int GetFocusSetCallCount(const Dali::WebEngine& webEngine);
bool GetLastFocus(const Dali::WebEngine& webEngine);
} // namespace WebEngine
} // namespace Test
