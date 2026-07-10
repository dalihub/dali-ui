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
#include <dali-ui-foundation/internal/text/text-pixel-snap-data.h>

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
namespace
{

constexpr const char* PIXEL_SNAP_FACTOR_PROPERTY_NAME    = "pixelSnapFactor";
const AttachmentId    TEXT_PIXEL_SNAP_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

TextPixelSnapData* GetTextPixelSnapData(View owner)
{
  if(!owner)
  {
    return nullptr;
  }

  return owner.GetAttachment<TextPixelSnapData>(TEXT_PIXEL_SNAP_DATA_ATTACHMENT_ID);
}

TextPixelSnapData& GetOrCreateTextPixelSnapData(View owner)
{
  DALI_ASSERT_ALWAYS(owner && "Text pixel snap attachment requires a valid owner");

  TextPixelSnapData* data = GetTextPixelSnapData(owner);
  if(!data)
  {
    owner.SetAttachment(TEXT_PIXEL_SNAP_DATA_ATTACHMENT_ID, Dali::MakeUnique<TextPixelSnapData>());
    data = GetTextPixelSnapData(owner);
  }

  DALI_ASSERT_ALWAYS(data && "Text pixel snap attachment creation failed");
  return *data;
}

} // namespace

Property::Index TextPixelSnapData::EnsurePixelSnapFactorProperty(View owner)
{
  if(!owner)
  {
    return Property::INVALID_INDEX;
  }

  if(mPixelSnapFactorPropertyIndex != Property::INVALID_INDEX)
  {
    return mPixelSnapFactorPropertyIndex;
  }

  const Property::Index existingIndex = owner.GetPropertyIndex(PIXEL_SNAP_FACTOR_PROPERTY_NAME);
  if(existingIndex != Property::INVALID_INDEX)
  {
    mPixelSnapFactorPropertyIndex = existingIndex;
    return mPixelSnapFactorPropertyIndex;
  }

  mPixelSnapFactorPropertyIndex = owner.RegisterProperty(PIXEL_SNAP_FACTOR_PROPERTY_NAME, 0.0f);
  return mPixelSnapFactorPropertyIndex;
}

Property::Index TextPixelSnapData::GetPixelSnapFactorPropertyIndex() const
{
  return mPixelSnapFactorPropertyIndex;
}

Property::Index EnsureTextPixelSnapFactorProperty(View owner)
{
  if(!owner)
  {
    return Property::INVALID_INDEX;
  }

  return GetOrCreateTextPixelSnapData(owner).EnsurePixelSnapFactorProperty(owner);
}

Property::Index GetTextPixelSnapFactorPropertyIndex(View owner)
{
  TextPixelSnapData*    data        = GetTextPixelSnapData(owner);
  const Property::Index storedIndex = data ? data->GetPixelSnapFactorPropertyIndex() : Property::INVALID_INDEX;
  if(storedIndex != Property::INVALID_INDEX || !owner)
  {
    return storedIndex;
  }

  return owner.GetPropertyIndex(PIXEL_SNAP_FACTOR_PROPERTY_NAME);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
