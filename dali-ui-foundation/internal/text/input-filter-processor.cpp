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
#include <dali-ui-foundation/internal/text/input-filter-processor.h>

// EXTERNAL INCLUDES
#include <regex>

namespace Dali
{
namespace Ui
{
namespace Text
{

InputFilterProcessor::InputFilterProcessor()
: mAllowPattern(),
  mDenyPattern()
{
}

void InputFilterProcessor::SetAllowPattern(const std::string& pattern)
{
  mAllowPattern = pattern;
}

const std::string& InputFilterProcessor::GetAllowPattern() const
{
  return mAllowPattern;
}

void InputFilterProcessor::SetDenyPattern(const std::string& pattern)
{
  mDenyPattern = pattern;
}

const std::string& InputFilterProcessor::GetDenyPattern() const
{
  return mDenyPattern;
}

bool InputFilterProcessor::IsAllowed(const std::string& source) const
{
  if(mAllowPattern.empty())
  {
    return true;
  }

  const std::regex pattern(mAllowPattern);
  return std::regex_match(source, pattern);
}

bool InputFilterProcessor::IsDenied(const std::string& source) const
{
  if(mDenyPattern.empty())
  {
    return false;
  }

  const std::regex pattern(mDenyPattern);
  return std::regex_match(source, pattern);
}

bool InputFilterProcessor::ApplyAllowPattern(std::string& text) const
{
  if(mAllowPattern.empty())
  {
    return false;
  }

  const std::regex pattern(mAllowPattern);
  std::string      result;

  auto current = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto end     = std::sregex_iterator();

  while(current != end)
  {
    result += current->str();
    ++current;
  }

  if(result == text)
  {
    return false;
  }

  text = result;
  return true;
}

bool InputFilterProcessor::ApplyDenyPattern(std::string& text) const
{
  if(mDenyPattern.empty())
  {
    return false;
  }

  const std::regex  pattern(mDenyPattern);
  const std::string result = std::regex_replace(text, pattern, "");

  if(result == text)
  {
    return false;
  }

  text = result;
  return true;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
