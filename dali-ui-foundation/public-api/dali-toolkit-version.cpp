/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali-ui-foundation/public-api/dali-toolkit-version.h>

// EXTERNAL INCLUDES
#ifdef DEBUG_ENABLED
#include <iostream>
#endif

namespace Dali
{
namespace UI
{
const unsigned int UI_MAJOR_VERSION = 2;
const unsigned int UI_MINOR_VERSION = 5;
const unsigned int UI_MICRO_VERSION = 11;
const char* const UI_BUILD_DATE = __DATE__ " " __TIME__;

#ifdef DEBUG_ENABLED
namespace
{
/// Allows the printing of the version number ONLY when debug is enabled
struct PrintVersion
{
  PrintVersion()
  {
    std::cerr << "DALi UI:   " << UI_MAJOR_VERSION << "." << UI_MINOR_VERSION << "." << UI_MICRO_VERSION << " ("
              << UI_BUILD_DATE << ")" << std::endl;
  }
};
PrintVersion UI_VERSION;
} // unnamed namespace
#endif

} // namespace UI

} // namespace Dali
