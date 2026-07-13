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
#include <dali-ui-foundation/internal/text/styled-text/styled-text-source-data.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

namespace
{

StyledTextSourceData& GetOrCreateStyledTextSourceData(StyledTextSourceDataPtr& data)
{
  if(!data)
  {
    data = std::make_unique<StyledTextSourceData>();
  }

  return *data;
}

} // namespace

void StyledTextSourceData::Set(const Dali::Ui::Text::StyledText& styledText)
{
  mStyledText = styledText;
}

void StyledTextSourceData::Clear()
{
  mStyledText = Dali::Ui::Text::StyledText();
}

Dali::Ui::Text::StyledText StyledTextSourceData::GetStyledText() const
{
  return mStyledText;
}

void SetStyledTextSource(StyledTextSourceDataPtr& data, const Dali::Ui::Text::StyledText& styledText)
{
  GetOrCreateStyledTextSourceData(data).Set(styledText);
}

void ClearStyledTextSource(StyledTextSourceDataPtr& data)
{
  if(data)
  {
    data->Clear();
  }
}

Dali::Ui::Text::StyledText GetStyledTextSource(const StyledTextSourceDataPtr& data)
{
  return data ? data->GetStyledText() : Dali::Ui::Text::StyledText();
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
