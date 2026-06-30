/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/property-map-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visuals-container.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali::Ui::Internal::Visual
{
namespace
{
DALI_ENUM_TO_STRING_TABLE_BEGIN(ALIGN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, TOP_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, TOP_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, TOP_END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, CENTER_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, CENTER_END)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, BOTTOM_BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, BOTTOM_CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Align, BOTTOM_END)
DALI_ENUM_TO_STRING_TABLE_END(ALIGN)

DALI_ENUM_TO_STRING_TABLE_BEGIN(POLICY)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Visual::Transform::Policy, RELATIVE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(Ui::Visual::Transform::Policy, ABSOLUTE)
DALI_ENUM_TO_STRING_TABLE_END(POLICY)

bool GetPolicyFromValue(const Property::Value& value, Vector2& policy)
{
  bool success = false;
  if(value.Get(policy))
  {
    success = true;
  }
  else
  {
    const Property::Array* array = value.GetArray();
    if(array && array->Size() == 2)
    {
      Ui::Visual::Transform::Policy::Type xPolicy =
        static_cast<Ui::Visual::Transform::Policy::Type>(-1); // Assign an invalid value so definitely changes
      Ui::Visual::Transform::Policy::Type yPolicy =
        static_cast<Ui::Visual::Transform::Policy::Type>(-1); // Assign an invalid value so definitely changes

      if(Scripting::GetEnumerationProperty<Ui::Visual::Transform::Policy::Type>(array->GetElementAt(0), POLICY_TABLE,
                                                                                POLICY_TABLE_COUNT, xPolicy) &&
         Scripting::GetEnumerationProperty<Ui::Visual::Transform::Policy::Type>(array->GetElementAt(1), POLICY_TABLE,
                                                                                POLICY_TABLE_COUNT, yPolicy))
      {
        policy.x = xPolicy;
        policy.y = yPolicy;
        success  = true;
      }
    }
  }
  return success;
}
} //namespace

Transform::Transform()
: mOffset(0.0f, 0.0f),
  mSize(1.0f, 1.0f),
  mExtraSize(0.0f, 0.0f),
  mOffsetSizeMode(0.0f, 0.0f, 0.0f, 0.0f),
  mOrigin(Ui::Align::TOP_BEGIN),
  mPivot(Ui::Align::TOP_BEGIN)
{
}

void Transform::SetPropertyMap(const Property::Map& map)
{
  // Set default values
  mOffset         = Vector2(0.0f, 0.0f);
  mSize           = Vector2(1.0f, 1.0f);
  mExtraSize      = Vector2(0.0f, 0.0f);
  mOffsetSizeMode = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
  mOrigin         = Ui::Align::TOP_BEGIN;
  mPivot          = Ui::Align::TOP_BEGIN;

  UpdatePropertyMap(map);
}

void Transform::UpdatePropertyMap(const Property::Map& map)
{
  for(Property::Map::SizeType i(0); i < map.Count(); ++i)
  {
    KeyValuePair keyValue = map.GetKeyValue(i);
    switch(Transform::GetIntKey(keyValue.first))
    {
      case Ui::Visual::Transform::Property::OFFSET:
      {
        keyValue.second.Get(mOffset);
        break;
      }
      case Ui::Visual::Transform::Property::SIZE:
      {
        keyValue.second.Get(mSize);
        break;
      }
      case Ui::Visual::Transform::Property::ORIGIN:
      {
        Scripting::GetEnumerationProperty<Ui::Align::Type>(keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, mOrigin);
        break;
      }
      case Ui::Visual::Transform::Property::PIVOT:
      {
        Scripting::GetEnumerationProperty<Ui::Align::Type>(keyValue.second, ALIGN_TABLE, ALIGN_TABLE_COUNT, mPivot);
        break;
      }
      case Ui::Visual::Transform::Property::OFFSET_POLICY:
      {
        Vector2 policy;
        if(GetPolicyFromValue(keyValue.second, policy))
        {
          mOffsetSizeMode.x = policy.x;
          mOffsetSizeMode.y = policy.y;
        }
        break;
      }
      case Ui::Visual::Transform::Property::SIZE_POLICY:
      {
        Vector2 policy;
        if(GetPolicyFromValue(keyValue.second, policy))
        {
          mOffsetSizeMode.z = policy.x;
          mOffsetSizeMode.w = policy.y;
        }
        break;
      }
      case Ui::Integration::Visual::Transform::Property::EXTRA_SIZE:
      {
        keyValue.second.Get(mExtraSize);
        break;
      }
    }
  }
}

void Transform::GetPropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Add(Ui::Visual::Transform::Property::OFFSET, mOffset)
    .Add(Ui::Visual::Transform::Property::SIZE, mSize)
    .Add(Ui::Visual::Transform::Property::ORIGIN, mOrigin)
    .Add(Ui::Visual::Transform::Property::PIVOT, mPivot)
    .Add(Ui::Visual::Transform::Property::OFFSET_POLICY, Vector2(mOffsetSizeMode.x, mOffsetSizeMode.y))
    .Add(Ui::Visual::Transform::Property::SIZE_POLICY, Vector2(mOffsetSizeMode.z, mOffsetSizeMode.w))
    .Add(Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, mExtraSize);
}

Vector2 Transform::GetVisualSize(const Vector2& controlSize, float viewEffectiveScale)
{
  return Vector2(Lerp(mOffsetSizeMode.z, mSize.x * controlSize.x, mSize.x * viewEffectiveScale),
                 Lerp(mOffsetSizeMode.w, mSize.y * controlSize.y, mSize.y * viewEffectiveScale)) +
         mExtraSize * viewEffectiveScale;
}

Property::Index Transform::GetIntKey(const Property::Key& key)
{
  if(key.type == Property::Key::INDEX)
  {
    return key.indexKey;
  }

  if(key.stringKey == PIVOT)
  {
    return Ui::Visual::Transform::Property::PIVOT;
  }
  else if(key.stringKey == EXTRA_SIZE)
  {
    return Ui::Integration::Visual::Transform::Property::EXTRA_SIZE;
  }
  else if(key.stringKey == OFFSET)
  {
    return Ui::Visual::Transform::Property::OFFSET;
  }
  else if(key.stringKey == OFFSET_POLICY)
  {
    return Ui::Visual::Transform::Property::OFFSET_POLICY;
  }
  else if(key.stringKey == ORIGIN)
  {
    return Ui::Visual::Transform::Property::ORIGIN;
  }
  else if(key.stringKey == SIZE)
  {
    return Ui::Visual::Transform::Property::SIZE;
  }
  else if(key.stringKey == SIZE_POLICY)
  {
    return Ui::Visual::Transform::Property::SIZE_POLICY;
  }
  return Property::INVALID_INDEX;
}

const Property::Map& Transform::GetDefaultTransformMap()
{
  static const Property::Map sDefaultTransformMap = Dali::CreatePropertyMap({
    {Ui::Visual::Transform::Property::OFFSET, Vector2::ZERO},
    {Ui::Visual::Transform::Property::SIZE, Vector2::ONE},
    {Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN},
    {Ui::Visual::Transform::Property::PIVOT, Ui::Align::TOP_BEGIN},
    {Ui::Visual::Transform::Property::OFFSET_POLICY, Vector2::ZERO},
    {Ui::Visual::Transform::Property::SIZE_POLICY, Vector2::ZERO},
    {Ui::Integration::Visual::Transform::Property::EXTRA_SIZE, Vector2::ZERO},
  });

  return sDefaultTransformMap;
}
} // namespace Dali::Ui::Internal::Visual
