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
#include <dali-ui-foundation/public-api/unique-any.h>

// EXTERNAL INCLUDES
#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#endif

#include <string>
#include <string_view>

namespace
{

std::string DemangleTypeInfoName(const char* typeIdName)
{
  if(!typeIdName || typeIdName[0] == '\0')
  {
    return {};
  }

#if defined(__GNUG__)
  int   status    = 0;
  char* demangled = abi::__cxa_demangle(typeIdName, nullptr, nullptr, &status);

  if(status == 0 && demangled != nullptr)
  {
    std::string result(demangled);
    free(demangled);
    return result;
  }

  if(demangled != nullptr)
  {
    free(demangled);
  }
  return std::string(typeIdName);
#else
  static const char* prefixes[] = {"class ", "struct ", "enum ", "union "};

  std::string name(typeIdName);
  for(const char* prefix : prefixes)
  {
    std::string_view p(prefix);
    if(name.compare(0, p.size(), prefix) == 0)
    {
      name.erase(0, p.size());
      break;
    }
  }

  return name;
#endif
}

} // unnamed namespace

namespace Dali
{
namespace Ui
{

UniqueAny::UniqueAny()
: mHolder(nullptr)
{
}

UniqueAny::UniqueAny(UniqueAny&& rhs) noexcept
: mHolder(rhs.mHolder)
{
  rhs.mHolder = nullptr;
}

UniqueAny& UniqueAny::operator=(UniqueAny&& rhs) noexcept
{
  if(this != &rhs)
  {
    Reset();
    mHolder     = rhs.mHolder;
    rhs.mHolder = nullptr;
  }

  return *this;
}

UniqueAny::~UniqueAny()
{
  Reset();
}

bool UniqueAny::Empty() const
{
  return !mHolder;
}

UniqueAny::operator bool() const
{
  return !Empty();
}

void UniqueAny::Reset()
{
  delete mHolder;
  mHolder = nullptr;
}

UniqueAny::HolderBase::~HolderBase() = default;

Dali::String UniqueAny::DemangleTypeName(const char* mangledName)
{
  if(!mangledName || mangledName[0] == '\0')
  {
    return Dali::String();
  }

  std::string demangled = DemangleTypeInfoName(mangledName);
  return Dali::String(demangled.c_str());
}

} // namespace Ui
} // namespace Dali
