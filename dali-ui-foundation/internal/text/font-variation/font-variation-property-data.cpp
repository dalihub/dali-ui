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
#include <dali-ui-foundation/internal/text/font-variation/font-variation-property-data.h>

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

const AttachmentId FONT_VARIATION_PROPERTY_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

} // namespace

bool FontVariationPropertyData::Insert(Dali::Property::Index index, const Dali::String& tag)
{
  return mVariationIndexMap.emplace(index, tag).second;
}

bool FontVariationPropertyData::Find(Dali::Property::Index index, Dali::String& tag) const
{
  auto iter = mVariationIndexMap.find(index);
  if(iter == mVariationIndexMap.end())
  {
    return false;
  }

  tag = iter->second;
  return true;
}

void FontVariationPropertyData::ApplyCurrentPropertyValues(Dali::Actor actor, Dali::Property::Map& map) const
{
  if(!actor)
  {
    return;
  }

  for(const auto& entry : mVariationIndexMap)
  {
    const Dali::Property::Index index = entry.first;
    const Dali::String&         tag   = entry.second;

    if(actor.DoesCustomPropertyExist(index))
    {
      float value = 0.0f;
      actor.GetCurrentProperty(index).Get(value);
      map[tag] = value;
    }
  }
}

FontVariationPropertyData* GetFontVariationPropertyData(Dali::Ui::View owner)
{
  if(!owner)
  {
    return nullptr;
  }

  return owner.GetAttachment<FontVariationPropertyData>(FONT_VARIATION_PROPERTY_DATA_ATTACHMENT_ID);
}

FontVariationPropertyData& GetOrCreateFontVariationPropertyData(Dali::Ui::View owner)
{
  DALI_ASSERT_ALWAYS(owner && "Font variation property attachment requires a valid owner");

  FontVariationPropertyData* data = GetFontVariationPropertyData(owner);
  if(!data)
  {
    owner.SetAttachment(FONT_VARIATION_PROPERTY_DATA_ATTACHMENT_ID, Dali::MakeUnique<FontVariationPropertyData>());
    data = GetFontVariationPropertyData(owner);
  }

  DALI_ASSERT_ALWAYS(data && "Font variation property attachment creation failed");
  return *data;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
