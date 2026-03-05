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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/hidden-text.h>
#include <dali-ui-foundation/internal/text/input-filter.h>

// INTERNAL INCLUDES

using namespace Dali::UI;

namespace Dali
{
namespace UI
{
namespace Text
{
const char* const PROPERTY_ACCEPTED = "accepted";
const char* const PROPERTY_REJECTED = "rejected";

InputFilter::InputFilter()
  : mAccepted(""),
    mRejected("")
{
}

void InputFilter::SetProperties(const Property::Map& map)
{
  const Property::Map::SizeType count = map.Count();

  for (Property::Map::SizeType position = 0; position < count; ++position)
  {
    KeyValuePair keyValue = map.GetKeyValue(position);
    Property::Key& key = keyValue.first;
    Property::Value& value = keyValue.second;

    if (key == UI::InputFilter::Property::ACCEPTED || key == PROPERTY_ACCEPTED)
    {
      value.Get(mAccepted);
    }
    else if (key == UI::InputFilter::Property::REJECTED || key == PROPERTY_REJECTED)
    {
      value.Get(mRejected);
    }
  }
}

void InputFilter::GetProperties(Property::Map& map)
{
  map[UI::InputFilter::Property::ACCEPTED] = mAccepted.c_str();
  map[UI::InputFilter::Property::REJECTED] = mRejected.c_str();
}

bool InputFilter::Contains(UI::InputFilter::Property::Type type, std::string source)
{
  bool match = false;
  std::regex pattern;

  if (type == UI::InputFilter::Property::ACCEPTED)
  {
    if (mAccepted.empty())
    {
      return true;
    }
    pattern = mAccepted;
  }
  else if (type == UI::InputFilter::Property::REJECTED)
  {
    if (mRejected.empty())
    {
      return false;
    }
    pattern = mRejected;
  }

  match = std::regex_match(source, pattern);

  return match;
}

bool InputFilter::Filter(UI::InputFilter::Property::Type type, std::string& text)
{
  std::regex pattern;
  std::string result;

  if (type == UI::InputFilter::Property::ACCEPTED)
  {
    if (mAccepted.empty())
    {
      return false;
    }
    pattern = mAccepted;

    auto start = std::sregex_iterator(text.begin(), text.end(), pattern);
    auto end = std::sregex_iterator();

    while (start != end)
    {
      result += start->str();
      ++start;
    }
  }
  else if (type == UI::InputFilter::Property::REJECTED)
  {
    if (mRejected.empty())
    {
      return false;
    }
    pattern = mRejected;

    result = std::regex_replace(text, pattern, "");
  }

  if (result.compare(text) == 0)
  {
    return false;
  }

  text = result;
  return true;
}

} // namespace Text

} // namespace UI

} // namespace Dali
