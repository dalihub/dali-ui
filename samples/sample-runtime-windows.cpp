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
 */

#include <windows.h>

#include <string>

namespace
{
bool SetWorkingDirectoryToExecutableDirectory()
{
  // The maximum extended-length path supported by the Windows API is 32,767
  // wide characters. GetModuleFileNameW does not require the path to exist in
  // the process' current working directory.
  std::wstring executablePath(32768u, L'\0');
  const DWORD  length = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
  if(length == 0u || length >= executablePath.size())
  {
    return false;
  }

  executablePath.resize(length);
  const std::wstring::size_type separator = executablePath.find_last_of(L"\\/");
  if(separator == std::wstring::npos)
  {
    return false;
  }

  executablePath.resize(separator);
  return SetCurrentDirectoryW(executablePath.c_str()) != FALSE;
}

// Resource paths are intentionally relative to the installed executable.
// Run this before main() so invoking a sample from any shell directory behaves
// exactly like launching it from dali-env/bin.
const bool gWorkingDirectoryInitialized = SetWorkingDirectoryToExecutableDirectory();

} // unnamed namespace
