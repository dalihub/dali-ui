#ifndef DALI_UI_SAMPLES_TEXT_LOCALIZATION_LOCALE_H
#define DALI_UI_SAMPLES_TEXT_LOCALIZATION_LOCALE_H

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

#include <clocale>
#include <cstdlib>
#include <string>

#if defined(_MSC_VER)
// Avoid including windows.h in this shared sample header because its global
// macros collide with DALi enum values such as ABSOLUTE.
extern "C" __declspec(dllimport) int __stdcall SetEnvironmentVariableA(const char* name, const char* value);
#endif

namespace Dali
{
namespace Ui
{
namespace Samples
{

inline bool SetMessageLocale(const char* locale)
{
  if(!locale)
  {
    return false;
  }

#if defined(_MSC_VER)
  // libintl is a DLL, so update both the Win32 process environment and the
  // executable's CRT environment before refreshing localization bindings.
  const bool processEnvironmentSet = SetEnvironmentVariableA("LANGUAGE", locale) != 0;
  const bool crtEnvironmentSet     = _putenv_s("LANGUAGE", locale) == 0;
  return processEnvironmentSet && crtEnvironmentSet;
#else
  return std::setlocale(LC_MESSAGES, locale) != nullptr;
#endif
}

inline const char* GetMessageLocale()
{
#if defined(_MSC_VER)
  thread_local std::string localeValue;
  char*                    environmentVariableValue = nullptr;
  std::size_t              valueLength              = 0u;
  if(_dupenv_s(&environmentVariableValue, &valueLength, "LANGUAGE") != 0 || environmentVariableValue == nullptr)
  {
    localeValue.clear();
    return "(null)";
  }

  localeValue.assign(environmentVariableValue);
  std::free(environmentVariableValue);
  const char* locale = localeValue.c_str();
#else
  const char* locale = std::setlocale(LC_MESSAGES, nullptr);
#endif
  return locale ? locale : "(null)";
}

} // namespace Samples
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_SAMPLES_TEXT_LOCALIZATION_LOCALE_H
