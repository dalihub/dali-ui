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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/traits/attachment-id.h>

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

const AttachmentId STYLED_TEXT_SOURCE_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

StyledTextSourceData* GetStyledTextSourceData(Dali::Ui::View owner)
{
  if(!owner)
  {
    return nullptr;
  }

  return owner.GetAttachment<StyledTextSourceData>(STYLED_TEXT_SOURCE_DATA_ATTACHMENT_ID);
}

StyledTextSourceData& GetOrCreateStyledTextSourceData(Dali::Ui::View owner)
{
  DALI_ASSERT_ALWAYS(owner && "StyledText source attachment requires a valid owner");

  StyledTextSourceData* data = GetStyledTextSourceData(owner);
  if(!data)
  {
    owner.SetAttachment(STYLED_TEXT_SOURCE_DATA_ATTACHMENT_ID, Dali::MakeUnique<StyledTextSourceData>());
    data = GetStyledTextSourceData(owner);
  }

  DALI_ASSERT_ALWAYS(data && "StyledText source attachment creation failed");
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

void SetStyledTextSource(Dali::Ui::View owner, const Dali::Ui::Text::StyledText& styledText)
{
  GetOrCreateStyledTextSourceData(owner).Set(styledText);
}

void ClearStyledTextSource(Dali::Ui::View owner)
{
  StyledTextSourceData* data = GetStyledTextSourceData(owner);
  if(data)
  {
    data->Clear();
  }
}

Dali::Ui::Text::StyledText GetStyledTextSource(Dali::Ui::View owner)
{
  StyledTextSourceData* data = GetStyledTextSourceData(owner);
  return data ? data->GetStyledText() : Dali::Ui::Text::StyledText();
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
