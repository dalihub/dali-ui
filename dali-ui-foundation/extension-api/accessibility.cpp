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
#include <dali-ui-foundation/extension-api/accessibility.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-service.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace Accessibility
{
bool SetScreenReaderSuppressed(bool suppressed)
{
  return Dali::Integration::Accessibility::SuppressScreenReader(suppressed);
}

void SayAccessibilityText(
  const Dali::String&   text,
  bool                  discardable,
  ReadingStatusCallback callback)
{
  const char*       rawText = text.CStr();
  const std::string message = rawText ? std::string(rawText, text.Size()) : std::string{};

  std::function<void(std::string)> integrationCallback;
  if(callback)
  {
    auto callbackHolder = std::make_shared<ReadingStatusCallback>(std::move(callback));
    integrationCallback = [callbackHolder](std::string status)
    {
      const Dali::String daliStatus(status.c_str());
      callbackHolder->Invoke(daliStatus);
    };
  }

  Dali::Integration::Accessibility::Say(message, discardable, std::move(integrationCallback));
}

} // namespace Accessibility
} // namespace Extension
} // namespace Ui
} // namespace Dali
